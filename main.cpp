#include <iostream>
#include <string>
#include <fstream>

#include "parallel_bnb.hpp"
#include "sequence_bnb.hpp"
#include "solver_provider.hpp"
#include "tsp.hpp"

#include "data_loader.h"

template <typename BNBSolver>
void solve(const std::string &problem_path, BNBSolver &solver)
{
	value_type *matrix;
	size_t rank;
	std::ifstream ifs(problem_path);
	load_tsplib_problem(ifs, matrix, rank);
	ifs.close();

	TspInitialData data(matrix, rank);
	
	double valuation_time = -1;
	value_type record = 0;
	try
	{
		Timer timer;
		record = solver.solve(data).value;
		valuation_time = timer.elapsed_seconds();
	}
	catch (std::bad_alloc &)
	{
		std::cout << "Out of memory\n";
	}
	delete[] matrix;
	
	solver.print_stats(std::cout);
	std::cout << "Found Record: " << record << std::endl;
	std::cout << "Valuation Time: " << valuation_time << std::endl;
		
}

int parallel(const std::string &problem_path)
{
	ClonedSolverProvider<TspSolver> provider;
	LoadBalancerParams params = {4, 8, 100};
	ParallelBNB<ClonedSolverProvider<TspSolver> > bnb(provider, params);

	solve(problem_path, bnb);
	
	return 0;
}

int sequnce(const std::string &problem_path)
{
	ClonedSolverProvider<TspSolver> provider;
	SequenceBNB<ClonedSolverProvider<TspSolver> > bnb(provider);

	solve(problem_path, bnb);
	return 0;
}

int main(int argc, char *argv[])
{
	std::cout << "starting..." << std::endl;
	std::string problem_path = "data/ftv38.atsp";
	if (argc > 1)
	{
		problem_path = argv[1];
	}
	//return sequnce(problem_path);
	return parallel(problem_path);
}
