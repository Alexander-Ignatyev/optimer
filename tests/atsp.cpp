#include <string>

#include <cppunit/extensions/HelperMacros.h>

#include <parallel_bnb.hpp>
#include <sequence_bnb.hpp>
#include <solver_provider.hpp>
#include <tsp.hpp>

#include <data_loader.h>

template <typename Solver>
void test_problem(Solver &solver, const TspInitialData &data, value_type expected_value) {
	value_type record = solver.solve(data).value;
	
	CPPUNIT_ASSERT_EQUAL(record, expected_value);
}

void test_sequential_problem_lifo(const TspInitialData &data, value_type expected_value) {
	ClonedSolverProvider<TspSolver> provider;
	SequenceBNB<ClonedSolverProvider<TspSolver>, LifoContainer > solver(provider);
	
	test_problem(solver, data, expected_value);
}

void test_sequential_problem_priority(const TspInitialData &data, value_type expected_value) {
	ClonedSolverProvider<TspSolver> provider;
	SequenceBNB<ClonedSolverProvider<TspSolver>, PriorityContainer> solver(provider);
	
	test_problem(solver, data, expected_value);
}

void test_parallel_problem_lifo(const TspInitialData &data, value_type expected_value) {
	ClonedSolverProvider<TspSolver> provider;
	LoadBalancerParams params = {4, 8, static_cast<unsigned>(data.rank*2)};
	ParallelBNB<ClonedSolverProvider<TspSolver>, LifoContainer > solver(provider, params);
	
	test_problem(solver, data, expected_value);
}

void test_parallel_problem_priority(const TspInitialData &data, value_type expected_value) {
	ClonedSolverProvider<TspSolver> provider;
	LoadBalancerParams params = {4, 8, static_cast<unsigned>(data.rank*2)};
	ParallelBNB<ClonedSolverProvider<TspSolver>, PriorityContainer > solver(provider, params);
	
	test_problem(solver, data, expected_value);
}


class ATSPTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( ATSPTest );
	CPPUNIT_TEST(test_sequential_FTV38_lifo);
	CPPUNIT_TEST(test_sequential_FTV38_priority);
	CPPUNIT_TEST(test_parallel_FTV38_lifo);
	CPPUNIT_TEST(test_parallel_FTV38_priority);
	CPPUNIT_TEST_SUITE_END();
	
	TspInitialData *ftv38_instance;
	value_type *ftv38_matrix;
	value_type fvt38_solution;
	
	void test_sequential_FTV38_lifo() {
		test_sequential_problem_lifo(*ftv38_instance, fvt38_solution);
	}
	
	void test_sequential_FTV38_priority() {
		test_sequential_problem_priority(*ftv38_instance, fvt38_solution);
	}
	
	void test_parallel_FTV38_lifo() {
		test_parallel_problem_lifo(*ftv38_instance, fvt38_solution);
	}
	
	void test_parallel_FTV38_priority() {
		test_parallel_problem_priority(*ftv38_instance, fvt38_solution);
	}
	
public:
	void setUp() {
		fvt38_solution = 1530;
		
		size_t dimension;
		std::ifstream ifs("data/ftv38.atsp");
		load_tsplib_problem(ifs, ftv38_matrix, dimension);
		ifs.close();
		
		ftv38_instance = new TspInitialData(ftv38_matrix, dimension);
	}
	
	void tearDown() {
		delete [] ftv38_matrix;
		delete ftv38_instance;
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ATSPTest );
