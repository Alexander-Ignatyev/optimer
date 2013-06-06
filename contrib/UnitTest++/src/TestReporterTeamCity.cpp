// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <sstream>

#include "TestReporterTeamCity.h"
#include "TestDetails.h"

namespace UnitTest {

namespace {
int secsToMillisecs(float secs)
{
    return static_cast<int>(secs*1000);
}
}

TestReporterTeamCity::TestReporterTeamCity()
    : m_flowid(JetBrains::getFlowIdFromEnvironment())
{
}

void TestReporterTeamCity::ReportFailure(TestDetails const& details, char const* failure)
{
    std::ostringstream oss;
    oss << failure << " at " << details.filename << ':' << details.lineNumber << std::endl;

    m_messages.testFailed(
        details.testName,
        failure,
        oss.str(),
        m_flowid
    );
}

void TestReporterTeamCity::ReportTestStart(TestDetails const& test)
{
    m_messages.testStarted(test.testName, m_flowid);
}

void TestReporterTeamCity::ReportTestFinish(TestDetails const& test, float seconds)
{
    m_messages.testFinished(test.testName, secsToMillisecs(seconds), m_flowid);
}

void TestReporterTeamCity::ReportSummary(int const /*totalTestCount*/, int const /*failedTestCount*/,
                                       int const /*failureCount*/, float /*secondsElapsed*/)
{
}

}
