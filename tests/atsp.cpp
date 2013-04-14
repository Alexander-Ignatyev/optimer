#include <string>

#include <cppunit/extensions/HelperMacros.h>

#include <parallel_bnb.hpp>
#include <sequence_bnb.hpp>
#include <solver_provider.hpp>
#include <tsp.hpp>

#include <data_loader.h>

void test_sequential_problem(const TspInitialData &data, value_type expected_value) {
	ClonedSolverProvider<TspSolver> provider;
	SequenceBNB<ClonedSolverProvider<TspSolver> > solver(provider);
	value_type record = solver.solve(data).value;
	
	CPPUNIT_ASSERT_EQUAL(record, expected_value);
}

void test_parallel_problem(const TspInitialData &data, value_type expected_value) {
	ClonedSolverProvider<TspSolver> provider;
	LoadBalancerParams params = {4, 8, static_cast<unsigned>(data.rank*2)};
	ParallelBNB<ClonedSolverProvider<TspSolver> > solver(provider, params);
	
	value_type record = solver.solve(data).value;
	
	CPPUNIT_ASSERT_EQUAL(record, expected_value);
}


class ATSPTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( ATSPTest );
	CPPUNIT_TEST(test_sequential_FTV38);
	CPPUNIT_TEST(test_parallel_FTV38);
	CPPUNIT_TEST_SUITE_END();
	
	TspInitialData *ftv38_instance;
	value_type *ftv38_matrix;
	value_type fvt38_solution;
	
	void test_sequential_FTV38() {
		test_sequential_problem(*ftv38_instance, fvt38_solution);
	}
	
	void test_parallel_FTV38() {
		test_parallel_problem(*ftv38_instance, fvt38_solution);
	}
	
public:
	void setUp()
	{
		fvt38_solution = 1530;
		
		size_t dimension;
		std::ifstream ifs("data/ftv38.atsp");
		load_tsplib_problem(ifs, ftv38_matrix, dimension);
		ifs.close();
		
		ftv38_instance = new TspInitialData(ftv38_matrix, dimension);
	}
	
	void tearDown()
	{
		delete [] ftv38_matrix;
		delete ftv38_instance;
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ATSPTest );
