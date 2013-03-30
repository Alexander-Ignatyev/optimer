#ifndef __PARALLEL_BNB__
#define __PARALLEL_BNB__

#include <pthread.h>
#include <iostream>
#include "load_balancer.h"


class mthreads_support
{
public:
	virtual void start(unsigned threadID) = 0;
};

struct thread_params
{
	mthreads_support *ptr;
	unsigned threadID;
};

void *task(void *ptr)
{
	thread_params *params = (thread_params *)ptr;
	params->ptr->start(params->threadID);
	return NULL;
}

template <typename SolverFactory>
class ParallelBNB: private mthreads_support
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

	nodes_t *m_nodes_list;
	MemoryManager<Set> m_mm;

	void start(unsigned threadID)
	{
		value_type record = m_record;
		Solution sol;
		nodes_t &nodes = m_nodes_list[threadID];
		Node<Set> *node;
		Solver *psolver = m_factory.get_solver();
		psolver->init(*m_idata, &m_mm);

		while(!nodes.empty())
		{
			node = nodes.top();
			nodes.pop();

			record = m_record;
			psolver->branch(node, record, nodes, sol);
			if(record < m_record)
				m_record = record;
		}
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

			while (!nodes.empty() && nodes.size()
					< m_params.minimum_nodes)
			{
				node = nodes.top();
				nodes.pop();

				psolver->branch(node, record, nodes, sol);
			}
			m_factory.free_solver(psolver);
		}

		//parallel part

		m_nodes_list = new nodes_t[num_threads];
		for (unsigned i = 0; !nodes.empty(); ++i)
		{
			m_nodes_list[i % num_threads].push(nodes.top());
			nodes.pop();
		}
		pthread_t *threads = new pthread_t[num_threads];
		thread_params *params = new thread_params[num_threads];
		for (unsigned i = 0; i < num_threads; ++i)
		{
			params[i].ptr = this;
			params[i].threadID = i;
			pthread_create(&threads[i], NULL, task, &params[i]);
		}

		for (unsigned i = 0; i < num_threads; ++i)
		{
			pthread_join(threads[i], NULL);
		}

		delete [] m_nodes_list;
		delete [] threads;
		delete [] params;
		std::cout << "end\n";
		return sol;
	}
};

#endif //__PARALLEL_BNB__
