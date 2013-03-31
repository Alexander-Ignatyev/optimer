#include <iostream>
#include <string>
#include <fstream>

#include <boost/progress.hpp>


#include "sequence_bnb.hpp"
#include "solver_provider.hpp"
#include "tsp.hpp"

#include "data_loader.h"

/*int parallel()
{
	value_type *matrix;
	size_t rank;
	std::ifstream ifs("ftv38.data");
	load_data(ifs, matrix, rank);
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
}*/

int sequnce()
{
	value_type *matrix;
	size_t rank;
	std::ifstream ifs("data/ftv38.atsp");
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
	std::cout << "End program he" << std::endl;
	return 0;
}

int main(int argc, char *argv[])
{
	std::cout << "starting..." << std::endl;
	return sequnce();
	//return parallel();
}
