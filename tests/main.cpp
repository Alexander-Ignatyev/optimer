// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "teamcity_cppunit.h"

int main(int argc, char *argv[]) {
    CppUnit::TestResult controller;
    CppUnit::TestResultCollector result;
    controller.addListener(&result);
    CppUnit::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    CppUnit::TestListener *listener;
    if (JetBrains::underTeamcity()) {
        listener = new JetBrains::TeamcityProgressListener();
    } else {
        listener = new CppUnit::BriefTestProgressListener();
    }
    controller.addListener(listener);
    runner.run(controller);
    delete listener;
    return result.wasSuccessful() ? 0 : 1;
}
