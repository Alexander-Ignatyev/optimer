/*
 * openmp_bnb.hpp
 *
 *  Created on: 06.11.2009
 *      Author: sash
 */

#ifndef OPENMP_BNB_HPP_
#define OPENMP_BNB_HPP_
#include <omp.h>
#include "load_balancer.h"

template <typename SolverFactory>
class OpenMP_BNB
{
	typedef typename SolverFactory::Solver Solver;
	typedef typename Solver::Set Set;
	typedef typename Solver::Solution Solution;
	typedef typename Solver::InitialData InitialData;
	typedef std::stack<Node<Set> *> nodes_t;

	SolverFactory &m_factory;
	const LoadBalancerParams &m_params;

	const InitialData *m_idata;
	value_type m_record;

	nodes_t *m_nodes_list;
	MemoryManager<Set> m_mm;


public:
	OpenMP_BNB(SolverFactory &factory, const LoadBalancerParams &params): m_factory(factory), m_params(params)
	{
	}

	Solution solve(InitialData &data, size_t max_branches = -1, value_type record = M_VAL)
	{
		static const size_t MIN_RANK_VALUE = 2;
		const unsigned num_threads = 8;
		Solution sol;
		nodes_t nodes;
		m_mm.init(m_idata->rank * m_idata->rank * 1024);
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

			while (!nodes.empty() && nodes.size() < m_params.minimum_nodes)
			{
				node = nodes.top();
				nodes.pop();

				psolver->branch(node, record, nodes, sol);
			}
			m_factory.free_solver(psolver);
		}


		return sol;
	}

};

#endif /* OPENMP_BNB_HPP_ */
