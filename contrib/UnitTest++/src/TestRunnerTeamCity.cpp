// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "TestRunner.h"
#include "TestReporterTeamCity.h"

namespace UnitTest {

int RunAllTestsWithTeamCity()
{
    if (JetBrains::underTeamcity()) {
        TestReporterTeamCity reporter;
        TestRunner runner(reporter);
        return runner.RunTestsIf(Test::GetTestList(), NULL, True(), 0);
    }
	else
    {
        return RunAllTests();
    }
}

}
