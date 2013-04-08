#ifndef __TSP_HPP__
#define __TSP_HPP__

#include <vector>
#include <stack>
#include <set>
#include <cassert>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tree.hpp"
#include "defs.h"
#include "ap_solver.hpp"
#include "stats.hpp"

bool is_m(int val)
{
	return val >= M_VAL;
}

bool is_m(double val)
{
	return val >= M_VAL;
}

struct TspInitialData
{
	const value_type *matrix;
	size_t rank;

	TspInitialData(value_type *m, size_t r): matrix(m), rank(r)
	{
	}
};
class TspSolver
{
public:
	typedef TspInitialData InitialData;
	struct Point
	{
		size_t x;
		size_t y;
	};

	struct Set
	{
		Point move;
		Point m_point;
		bool is_right;
		value_type value;
		std::vector<size_t> ap_solve;
		unsigned level;
	};

	struct Solution
	{
		value_type value;
		std::vector<size_t> route;
	};
private:
	MemoryManager<Set> *m_mm;
	std::ostream &logger;
	size_t rank;
	size_t max_branches;
	value_type *m_mtx;
	const value_type *m_mtx_original;
	AP_Solver<value_type> ap;

private:
	static std::vector<size_t> create_tour(const std::vector<size_t> &ap_sol, std::ostream &logger = std::cout)
	{
		std::vector<size_t> tour;
		size_t start = 0;
		tour.push_back(start);
		size_t finish = ap_sol[start];
		tour.push_back(finish);
		size_t counter = 0;
		while(start != finish && counter < 1000)
		{
			++counter;
			finish = ap_sol[finish];
			tour.push_back(finish);
		}
		if(counter > 999)
		{
			tour.clear();
		}
		return tour;
	}


	static void get_greedy_solution(const value_type *data, size_t rank,
			Solution &sol, unsigned startPoint)
	{
		sol.value = 0;

		std::set<size_t> points;
		for (size_t i = 0; i < rank; ++i)
			points.insert(i);
		size_t currPoint = startPoint;
		size_t minPoint;
		value_type minValue = M_VAL + 10;
		std::set<size_t>::iterator pos;

		points.erase(currPoint);
		sol.route.push_back(currPoint);
		while (!points.empty())
		{
			minValue = M_VAL + 10;
			for (pos = points.begin(); pos != points.end(); ++pos)
			{
				if (data[currPoint*rank+*pos] < minValue)
				{
					minValue = data[currPoint*rank+*pos];
					minPoint = *pos;
				}
			}
			sol.value += minValue;
			points.erase(minPoint);//[[ru:������, �����]]
 			currPoint = minPoint;
			sol.route.push_back(currPoint);
		}
		sol.value += data[currPoint*rank+startPoint];
		sol.route.push_back(startPoint);
	}

	bool two_opt(Solution &sol) const
	{
		bool bResult = false;
		const value_type *mtx = m_mtx_original;
		bool bContinue = true;

		while (bContinue)
		{
			bContinue = false;
			for (unsigned first = 1, second = 2; second < sol.route.size()
					- 2; ++first, ++second)
			{
				value_type delta = (mtx[sol.route[first - 1]*rank+sol.route[first]] + 
					mtx[sol.route[first]*rank+sol.route[second]] + 
					mtx[sol.route[second]*rank+sol.route[second + 1]]) - 
					(mtx[sol.route[first- 1]*rank+sol.route[second]] + 
					mtx[sol.route[second]*rank+sol.route[first]] + 
					mtx[sol.route[first]*rank+sol.route[second + 1]]);
				if (delta > 0)
				{
					std::swap(sol.route[first], sol.route[second]);
					sol.value -= delta;
					bResult = true;
					bContinue = true;
				}
			}
		}
		return bResult;
	}

	static void print_matrix(const value_type *matrix, size_t rank, std::ostream &logger = std::cout)
	{
		for(size_t i = 0; i < rank; ++i)
		{
			for(size_t j = 0; j < rank; ++j)
			{
				if(i != j && !is_m(matrix[i*rank+j]))
				{
					logger << matrix[i*rank+j] << " ";
				}
				else
				{
					logger << "M ";
				}   
			}
			logger << std::endl;
		}
		logger << std::endl;
	}

	static void copy_matrix(value_type *target, const value_type *source, size_t rank, const Node<Set> *pnode)
	{
		memcpy(target, source, rank*rank*sizeof(value_type));
		if(pnode->parent)
		{
			target[pnode->data.m_point.x*rank+pnode->data.m_point.y] = M_VAL;
			while(pnode->parent)
			{
				if(pnode->data.is_right)
				{
					for(unsigned k = 0; k < rank; ++k)
					{
						target[rank*pnode->data.move.x+k] = M_VAL;
						target[rank*k+pnode->data.move.y] = M_VAL;
					}
				}
				else
				{
					target[pnode->data.m_point.x*rank+pnode->data.m_point.y] = M_VAL;
				}
				target[pnode->data.m_point.x*rank+pnode->data.m_point.y] = M_VAL;
				pnode = pnode->parent;
			}
		}
	}


	bool select_move(const value_type *data, const Node<Set> &node, Point &move)
	{
		bool move_selected = false;
		value_type theta = 0;

		for(size_t i = 0; i < node.data.ap_solve.size(); ++i)
		{
			size_t j = node.data.ap_solve[i];
			value_type min_i = M_VAL;
			value_type min_j = M_VAL;
			value_type val = data[i*rank+j];
			if(is_m(val))
			{
				continue;
			}
			for (unsigned k = 0; k < rank; ++k)
			{
				if (k != j)
				{
					if (min_i > data[i *rank+k])
					{
						min_i = data[i *rank+k];
					}
				}
				if (k != i)
				{
					if (min_j > data[k*rank+j])
					{
						min_j = data[k*rank+j];
					}
				}
			}
			if (!is_m(min_i) && !is_m(min_j))
			{
				if (!move_selected || (theta < (min_i + min_j - 2*val)))
				{
					theta = min_i + min_j - 2*val;
					move.x = i;
					move.y = j;
					move_selected = true;
				}
			}
		}
		return move_selected;
	}

	value_type transform(Node<Set> &node, const value_type *data)
	{
		std::vector<size_t> markIndices = ap.transform(data, rank);
		node.data.ap_solve.resize(rank);
		for (size_t i = 0; i < rank; ++i)
		{
			node.data.ap_solve[markIndices[i]] = i;
		}

		value_type d0 = 0;
		for(unsigned i = 0; i < rank; ++i)
		{
			if(!is_m(data[markIndices[i]*rank+i]))
			{
				d0 += data[markIndices[i]*rank+i];
			}
		}
		const Node<Set> *pnode = &node;
		while(pnode)
		{
			if(pnode->data.is_right)
			{
				d0 += m_mtx_original[pnode->data.move.x*rank+pnode->data.move.y];
				node.data.ap_solve[pnode->data.move.x] = pnode->data.move.y;
			}
			pnode = pnode->parent;
		}
		return d0;
	}
	
	void anti_cycle(Node<Set> &node)
	{
		bool bContinue = true;
		size_t start, finish;
		start = node.data.move.x;
		finish = node.data.move.y;
		while(bContinue)
		{
			bContinue = false;
			const Node<Set> *pnode = node.parent;
			while(pnode)
			{
				if(pnode->data.is_right)
				{
					if(pnode->data.move.x == finish)
					{
						finish = pnode->data.move.y;
						bContinue = true;
						break;
					}
					if(pnode->data.move.y == start)
					{
						start = pnode->data.move.x;
						bContinue = true;
						break;
					}
				}
				pnode = pnode->parent;
			}
		}
		node.data.m_point.x = finish;
		node.data.m_point.y = start;
	}
public:

	//mandatory function
	void init(const TspInitialData &data, MemoryManager<Set> *mm)
	{
		m_mtx_original = data.matrix;
		rank = data.rank;
		ap.alloc(rank);
		m_mm = mm;
		m_mtx = new value_type[rank*rank];
	}

	void get_initial_node(Node<Set> &node)
	{
		node.data.level = 0;
		node.data.is_right = false;
		node.data.value = transform(node, m_mtx_original);
	}

	TspSolver(std::ostream &_logger = std::cout): logger(_logger)
	{
	}

	void get_initial_solution(Solution &sol)
	{
		Solution tmpSol;
		sol.value = M_VAL;
		for (unsigned i = 0; i < rank; ++i)
		{
			tmpSol.value = 0;
			tmpSol.route.clear();
			get_greedy_solution(m_mtx_original, rank, tmpSol, i);
			two_opt(tmpSol);
			if (sol.value > tmpSol.value)
			{
				sol = tmpSol;
			}
		}
		logger << std::endl;
	}

	void branch(const Node<Set> *node, value_type &record, std::stack<Node<Set> *> &nodes, Solution &sol, Stats &stats)
	{
		stats.branches++;
		Point move;
		copy_matrix(m_mtx, m_mtx_original, rank, node);
		if(!select_move(m_mtx, *node, move))
		{
			logger << "TSP AP: Error! Cannot select move" << std::endl;
			return;
		}

		bool append_move_right = false;

		Node<Set> *node1 = m_mm->alloc(node);
		node1->data.level = node->data.level+1;
		node1->data.move = move;
		node1->data.is_right = true;
		anti_cycle(*node1);
		copy_matrix(m_mtx, m_mtx_original, rank, node1);
		node1->data.value = transform(*node1, m_mtx);
		assert(m_mm->CheckRefs(node));
		++stats.sets_generated;
		if(node1->data.value < record)
		{
			if(node1->data.level == rank - 2)
			{
				record = node1->data.value;
				logger << "TSP AP: found new record: " << record << " "<< node1->data.level << std::endl;
				sol.route = create_tour(node1->data.ap_solve);
#ifndef NDEBUG
				if(sol.route.empty())
				{
					std::cerr << "error AP solve\n";
					for(size_t i = 0; i < node1->data.ap_solve.size(); ++i)
					{
						std::cout << "(" << i << ", " << node1->data.ap_solve[i] << ") --- ";
					}
					std::cout << std::endl;
					node = node1;
					while(node->parent)
					{
						if(node->data.is_right)
						{
							std::cout << "(" << node->data.move.x << ", " << node->data.move.y << ")\n";
						}
						node = const_cast<Node<Set> *>(node->parent);
					}
					print_matrix(m_mtx, rank, std::cout);
					std::cout << std::endl;
					exit(2);
				}
#endif

				sol.value = record;
				m_mm->free(node1);
				return;
			}
			append_move_right = true;
		}
		else
		{
			++stats.sets_constrained_by_record;
		}

		Node<Set> *node2 = m_mm->alloc(node);
		node2->data.level = node->data.level;
		node2->data.m_point = move;
		node2->data.is_right = false;
		copy_matrix(m_mtx, m_mtx_original, rank, node2);
		node2->data.value = transform(*node2, m_mtx);
		++stats.sets_generated;
		if(node2->data.value < record)
		{
			nodes.push(node2);
		}
		else
		{
			++stats.sets_constrained_by_record;
			m_mm->free(node2);
		}

		if(append_move_right)
		{
			nodes.push(node1);
		}
		else
		{
			m_mm->free(node1);
		}
	}
};
#endif //__TSP_HPP__
