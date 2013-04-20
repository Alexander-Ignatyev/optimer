/*
 * solver_factory.hpp
 *
 *  Created on: Oct 21, 2009
 *      Author: sash
 */

template <typename S>
class ClonedSolverProvider
{
public:
	typedef S Solver;

public:
	static Solver *get_solver()
	{
		return new Solver();
	}

	static void free_solver(Solver *solver)
	{
		delete solver;
	}
};
