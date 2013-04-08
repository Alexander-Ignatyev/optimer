#include <iostream>
#include <string>
#include <fstream>

#include <boost/progress.hpp>

#include "parallel_bnb.hpp"
#include "sequence_bnb.hpp"
#include "solver_provider.hpp"
#include "tsp.hpp"

#include "data_loader.h"

int parallel(const std::string &problem_path)
{
	value_type *matrix;
	size_t rank;
	std::ifstream ifs(problem_path);
	load_tsplib_problem(ifs, matrix, rank);
	ifs.close();

	TspInitialData data(matrix, rank);

	boost::progress_timer prg;
	ClonedSolverProvider<TspSolver> provider;
	LoadBalancerParams params = {8};
	ParallelBNB<ClonedSolverProvider<TspSolver> > bnb(provider, params);

	try
	{
		bnb.solve(data);
	} catch (std::bad_alloc &)
	{
		std::cout << "Out of memory\n";
		return 2;
	}
	delete[] matrix;
	return 0;
}

int sequnce(const std::string &problem_path)
{
	value_type *matrix;
	size_t rank;
	std::ifstream ifs(problem_path);
	load_tsplib_problem(ifs, matrix, rank);
	ifs.close();

	TspInitialData data(matrix, rank);

	boost::progress_timer prg;
	ClonedSolverProvider<TspSolver> provider;
	SequenceBNB<ClonedSolverProvider<TspSolver> > bnb(provider);

	try
	{
		bnb.solve(data);
	}
	catch (std::bad_alloc &)
	{
		std::cout << "Out of memory\n";
		return 2;
	}
	delete[] matrix;
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
