#ifndef __PARALLEL_BNB__
#define __PARALLEL_BNB__

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>

#include <thread>
#include <condition_variable>

#include "defs.h"
#include "bnb.hpp"
#include "tree.hpp"
#include "stats.h"
#include "timer.hpp"
#include "load_balancer.h"

template <typename SolverFactory, typename NodesContainer = LifoContainer>
class ParallelBNB
{
	typedef typename SolverFactory::Solver Solver;
	typedef typename Solver::Set Set;
	typedef typename Solver::Solution Solution;
	typedef typename Solver::InitialData InitialData;

	const LoadBalancerParams &m_params;
	typedef std::vector<Node<Set> *> nodes_t;

	const InitialData *m_idata;
	
	volatile value_type m_record;
	std::mutex m_mutex_record;
	
	volatile unsigned m_working_threads;
	std::queue<Node<Set> *> m_queue_sets;
	std::mutex m_mutex_sets;
	std::condition_variable m_condvar_sets;

	std::vector<nodes_t> m_nodes_list;
	std::vector<Stats> m_stats_list;
	Stats m_stats_initial;
	MemoryManager<Set> m_mm;

	void start(unsigned threadID)
	{
		m_stats_list[threadID].clear();
		value_type record = m_record;
		Solution sol;
		auto nodes = make_nodes_container<SolverFactory>(LifoContainer(), m_nodes_list[threadID].begin(), m_nodes_list[threadID].end());
		m_nodes_list[threadID].clear();
		Node<Set> *node;
		Solver *psolver = SolverFactory::get_solver();
		psolver->init(*m_idata, &m_mm);
		
		std::vector<Node<Set> * > tmp_nodes;

		Timer timer;
		while(!nodes.empty())
		{
			node = nodes.top();
			nodes.pop();

			record = m_record;
			psolver->branch(node, record, tmp_nodes, sol, m_stats_list[threadID]);
			for (auto &set: tmp_nodes)
			{
				nodes.push(set);
			}
			tmp_nodes.clear();
			
			if(record < m_record)
			{
				std::lock_guard<std::mutex> lock(m_mutex_record);
				if(record < m_record)
				{
					m_record = record;
				}
			}
			
			if (nodes.size() > m_params.maximum_nodes)
			{
				std::lock_guard<std::mutex> lock(m_mutex_sets);
				while(nodes.size() > m_params.minimum_nodes)
				{
					m_queue_sets.push(nodes.top());
					nodes.pop();
					++m_stats_list[threadID].sets_sent;
				}
				m_condvar_sets.notify_all();
			}
			
			if (nodes.empty())
			{
				m_stats_list[threadID].seconds += timer.elapsed_seconds();
				std::unique_lock<std::mutex> lock(m_mutex_sets);
				--m_working_threads;
				m_condvar_sets.wait(lock, [this] { return (!this->m_queue_sets.empty()) || (this->m_working_threads == 0); });
				if (m_working_threads == 0)
				{
					m_condvar_sets.notify_all();
					timer.reset();
					break;
				}
				while (nodes.size() < m_params.minimum_nodes && !m_queue_sets.empty())
				{
					nodes.push(m_queue_sets.front());
					m_queue_sets.pop();
					++m_stats_list[threadID].sets_received;
				}
				++m_working_threads;
				timer.reset();
			}
		}
		m_stats_list[threadID].seconds += timer.elapsed_seconds();
		SolverFactory::free_solver(psolver);
	}

public:
	ParallelBNB(const LoadBalancerParams &params): m_params(params), m_record(0), m_working_threads(0)
	{
	}

	Solution solve(const InitialData &data, size_t max_branches =
			-1, value_type record = M_VAL)
	{
		static const size_t MIN_RANK_VALUE = 2;

		m_record = record;
		m_idata = &data;

		Solution sol;
		auto nodes = make_nodes_container<SolverFactory>(LifoContainer());
		m_mm.init(data.rank*data.rank*data.rank*data.rank);
		m_stats_initial.clear();
		if (data.rank > MIN_RANK_VALUE)
		{
			Solver *psolver = SolverFactory::get_solver();
			psolver->init(data, &m_mm);
			Node<Set> *node = m_mm.alloc(NULL);
			psolver->get_initial_node(node);
			nodes.push(node);

			Solution initSol;
			psolver->get_initial_solution(&initSol);
			m_record = initSol.value;

			std::vector<Node<Set> * > tmp_nodes;

			Timer timer;
			while (!nodes.empty() && nodes.size()
					< m_params.minimum_nodes)
			{
				node = nodes.top();
				nodes.pop();

				psolver->branch(node, record, tmp_nodes, sol, m_stats_initial);
				for (auto &set: tmp_nodes)
				{
					nodes.push(set);
				}
				tmp_nodes.clear();
			}
			m_stats_initial.seconds = timer.elapsed_seconds();
			SolverFactory::free_solver(psolver);
		}

		//parallel part
		m_nodes_list.resize(m_params.threads);
		m_stats_list.resize(m_params.threads);
		for (unsigned i = 0; !nodes.empty(); ++i)
		{
			m_nodes_list[i % m_params.threads].push_back(nodes.top());
			nodes.pop();
		}
		
		std::vector<std::thread> threads(m_params.threads);
		for (unsigned i = 0; i < m_params.threads; ++i)
		{
			++m_working_threads;
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
