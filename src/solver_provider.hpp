/*
 * solver_factory.hpp
 *
 *  Created on: Oct 21, 2009
 *      Author: sash
 */

template <typename S>
class SingleSolverProvider
{
public:
	typedef S Solver;

private:
	Solver m_solver;
public:
	SingleSolverProvider()
	{
	}

	Solver *get_solver()
	{
		return &m_solver;
	}

	void free_solver(Solver *solver)
	{
	}
};

template <typename S>
class ClonedSolverProvider
{
public:
	typedef S Solver;

public:
	Solver *get_solver()
	{
		return new Solver();
	}

	void free_solver(Solver *solver)
	{
		delete solver;
	}
};
