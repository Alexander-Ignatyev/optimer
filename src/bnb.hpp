#ifndef __BNB_HPP__
#define __BNB_HPP__

#include <stack>
#include <queue>

#include "defs.h"
#include "tree.hpp"

struct LifoContainer { };

struct PriorityContainer { };

template<typename SolverProvider>
std::stack<Node<typename SolverProvider::Solver::Set> *> make_nodes_container(LifoContainer lifo)
{
	return std::stack<Node<typename SolverProvider::Solver::Set> *>();
}

template<typename SolverProvider, typename InputIterator>
std::stack<Node<typename SolverProvider::Solver::Set> *> make_nodes_container(LifoContainer lifo, InputIterator first, InputIterator last)
{
	auto container = std::stack<Node<typename SolverProvider::Solver::Set> *>();
	for (; first != last; ++first)
	{
		container.push(*first);
	}
	
	return std::move(container);
}
// , PtrNodeComparer<typename SolverProvider::Solver::Set>

template<typename Set>
struct PtrNodeComparer
{
	bool operator () (const Node<Set> *lhs, const Node<Set> *rhs) const
	{
		return lhs->data < rhs->data; 
	}
};

template<typename SolverProvider>
std::priority_queue<Node<typename SolverProvider::Solver::Set> *, std::vector<Node<typename SolverProvider::Solver::Set> *>, PtrNodeComparer<typename SolverProvider::Solver::Set> > make_nodes_container(PriorityContainer priority)
{
	return std::priority_queue<Node<typename SolverProvider::Solver::Set> *, std::vector<Node<typename SolverProvider::Solver::Set> *>, PtrNodeComparer<typename SolverProvider::Solver::Set> >();
}

template<typename SolverProvider, typename InputIterator>
std::priority_queue<Node<typename SolverProvider::Solver::Set> *, std::vector<Node<typename SolverProvider::Solver::Set> *>, PtrNodeComparer<typename SolverProvider::Solver::Set> > make_nodes_container(PriorityContainer priority, InputIterator first, InputIterator last)
{
	return std::priority_queue<Node<typename SolverProvider::Solver::Set> *, std::vector<Node<typename SolverProvider::Solver::Set> *>, PtrNodeComparer<typename SolverProvider::Solver::Set> >() (first, last);
}

#endif //__BNB_HPP__
