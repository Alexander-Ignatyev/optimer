#ifndef __PARALLEL_BNB__
#define __PARALLEL_BNB__

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

#include <thread>

#include "defs.h"
#include "tree.hpp"
#include "stats.hpp"
#include "timer.hpp"
#include "load_balancer.h"

template <typename SolverFactory>
class ParallelBNB
{
	typedef typename SolverFactory::Solver Solver;
	typedef typename Solver::Set Set;
	typedef typename Solver::Solution Solution;
	typedef typename Solver::InitialData InitialData;

	SolverFactory &m_factory;
	const LoadBalancerParams &m_params;
	typedef std::stack<Node<Set> *> nodes_t;

	const InitialData *m_idata;
	
	value_type m_record;
	std::mutex m_mutex_record;

	std::vector<nodes_t> m_nodes_list;
	std::vector<Stats> m_stats_list;
	Stats m_stats_initial;
	MemoryManager<Set> m_mm;

	void start(unsigned threadID)
	{
		m_stats_list[threadID].clear();
		value_type record = m_record;
		Solution sol;
		nodes_t &nodes = m_nodes_list[threadID];
		Node<Set> *node;
		Solver *psolver = m_factory.get_solver();
		psolver->init(*m_idata, &m_mm);

		Timer timer;
		while(!nodes.empty())
		{
			node = nodes.top();
			nodes.pop();

			record = m_record;
			psolver->branch(node, record, nodes, sol, m_stats_list[threadID]);
			if(record < m_record)
			{
				std::lock_guard<std::mutex> lock(m_mutex_record);
				if(record < m_record)
				{
					m_record = record;
				}
			}
		}
		m_stats_list[threadID].seconds = timer.elapsed_seconds();
		m_factory.free_solver(psolver);
	}

public:
	ParallelBNB(SolverFactory &factory, const LoadBalancerParams &params): m_factory(factory), m_params(params)
	{
	}

	Solution solve(InitialData &data, size_t max_branches =
			-1, value_type record = M_VAL)
	{
		static const size_t MIN_RANK_VALUE = 2;
		const unsigned num_threads = 8;

		m_record = record;
		m_idata = &data;

		Solution sol;
		nodes_t nodes;
		m_mm.init(m_idata->rank*m_idata->rank*1024);
		m_stats_initial.clear();
		if (data.rank > MIN_RANK_VALUE)
		{
			Solver *psolver = m_factory.get_solver();
			psolver->init(data, &m_mm);
			Node<Set> *node = m_mm.alloc(NULL);
			psolver->get_initial_node(*node);
			nodes.push(node);

			Solution initSol;
			psolver->get_initial_solution(initSol);
			m_record = initSol.value;

			Timer timer;
			while (!nodes.empty() && nodes.size()
					< m_params.minimum_nodes)
			{
				node = nodes.top();
				nodes.pop();

				psolver->branch(node, record, nodes, sol, m_stats_initial);
			}
			m_stats_initial.seconds = timer.elapsed_seconds();
			m_factory.free_solver(psolver);
		}

		//parallel part
		m_nodes_list.resize(num_threads);
		m_stats_list.resize(num_threads);
		for (unsigned i = 0; !nodes.empty(); ++i)
		{
			m_nodes_list[i % num_threads].push(nodes.top());
			nodes.pop();
		}
		
		std::vector<std::thread> threads(num_threads);
		for (unsigned i = 0; i < num_threads; ++i)
		{
			threads[i] = std::thread(&ParallelBNB::start, this, i);
		}

		std::for_each(threads.begin(),threads.end(),
		    std::mem_fn(&std::thread::join));
		
		m_nodes_list.clear();

		sol.value = m_record;
		sol.route.clear();
		return std::move(sol);
	}
	
	void print_stats(std::ostream &os) const
	{
		os << std::endl;
		
		os << "Initial stats:\n" << m_stats_initial << std::endl;

		for (size_t i = 0; i < m_stats_list.size(); ++i)
		{
			os << "Stats of theread #" << i << ":" << std::endl;
			os << m_stats_list[i] << std::endl;
		}
	}
};

#endif //__PARALLEL_BNB__
