// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef UNITTEST_TESTREPORTERSTDOUT_H
#define UNITTEST_TESTREPORTERSTDOUT_H

#include <string>

#include "TestReporter.h"
#include "teamcity_messages.h"

namespace UnitTest {

class TestReporterTeamCity : public TestReporter
{
public:
    TestReporterTeamCity();
private:
    virtual void ReportTestStart(TestDetails const& test);
    virtual void ReportFailure(TestDetails const& test, char const* failure);
    virtual void ReportTestFinish(TestDetails const& test, float secondsElapsed);
    virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);
    
    std::string m_flowid;
    JetBrains::TeamcityMessages m_messages;
};

}

#endif 
