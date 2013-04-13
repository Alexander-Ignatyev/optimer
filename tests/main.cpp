#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "teamcity_cppunit.h"

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
