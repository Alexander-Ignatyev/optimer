#ifndef __SEQUENCE_BNB__
#define __SEQUENCE_BNB__

#include <stack>
#include "defs.h"
#include "tree.hpp"
#include <iostream>

template <typename SolverProvider>
class SequenceBNB
{
	typedef typename SolverProvider::Solver Solver;
	typedef typename Solver::Set Set;
	typedef typename Solver::Solution Solution;
	typedef typename Solver::InitialData InitialData;

	SolverProvider &m_provider;
public:
	SequenceBNB(SolverProvider &provider): m_provider(provider)
	{
	}

	Solution solve(InitialData &data, size_t max_branches = -1, value_type record = M_VAL)
	{
		static const size_t MIN_RANK_VALUE = 2;

		Solution sol; 
		if(data.rank > MIN_RANK_VALUE)
		{
			MemoryManager<Set> mm;
			mm.init(data.rank*data.rank);
			Node<Set> *node = mm.alloc(NULL);
			Solution initSol;

			Solver *solver = m_provider.get_solver();
			solver->init(data, &mm);
			solver->get_initial_node(*node);
			solver->get_initial_solution(initSol);
			record = initSol.value;

			std::stack<Node<Set> * > nodes;
			nodes.push(node);
			while(!nodes.empty() /*&& this->stats.branches < max_branches*/)
			{
				node = nodes.top();
				nodes.pop();

				solver->branch(node, record, nodes, sol);
			}
		}
		return sol;
	}
};
#endif //__SEQUENCE_BNB__
