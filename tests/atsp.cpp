#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <iostream>
#include <string>
#include <fstream>

#include "parallel_bnb.hpp"
#include "sequence_bnb.hpp"
#include "solver_provider.hpp"
#include "tsp.hpp"

#include "data_loader.h"

class ATSPTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( ATSPTest );
	CPPUNIT_TEST(testFTV38);
	CPPUNIT_TEST_SUITE_END();
	
	void test_problem(const std::string &problem_path, value_type expected_value) {
		value_type *matrix;
		size_t rank;
		std::ifstream ifs(problem_path);
		load_tsplib_problem(ifs, matrix, rank);
		ifs.close();
		
		TspInitialData data(matrix, rank);
		
		ClonedSolverProvider<TspSolver> provider;
		SequenceBNB<ClonedSolverProvider<TspSolver> > solver(provider);
		value_type record = solver.solve(data).value;
		delete[] matrix;
		
		CPPUNIT_ASSERT_EQUAL(record, expected_value);
	}
	
	void testFTV38() {
		test_problem("data/ftv38.atsp", 1530);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ATSPTest );

int main(int argc, char *argv[]) {
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    bool wasSuccessful = runner.run( "", false );
    return wasSuccessful;
}
