#include <iostream>
#include <string>
#include <fstream>

#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <parallel_bnb.hpp>
#include <sequence_bnb.hpp>
#include <solver_provider.hpp>
#include <tsp.hpp>

#include <data_loader.h>

#include "teamcity_cppunit.h"

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
	using namespace CppUnit;
    // Create the event manager and test controller
    TestResult controller;
    
    // Add a listener that collects test result
    TestResultCollector result;
    controller.addListener(&result);
    
    // Add the top suite to the test runner
    TestRunner runner;
    runner.addTest(TestFactoryRegistry::getRegistry().makeTest());

    // Listen to progress
    TestListener *listener;
    
    if (JetBrains::underTeamcity()) {
        // Add unique flowId parameter if you want to run test processes in parallel
        // See http://confluence.jetbrains.net/display/TCD6/Build+Script+Interaction+with+TeamCity#BuildScriptInteractionwithTeamCity-MessageFlowId
        listener = new JetBrains::TeamcityProgressListener();
    } else {
        listener = new BriefTestProgressListener();
    }
    controller.addListener(listener);

    // Run tests
    runner.run(controller);
    
    delete listener;
    
    return result.wasSuccessful() ? 0 : 1;
    
}
