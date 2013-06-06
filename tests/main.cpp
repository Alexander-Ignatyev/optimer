// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <g2log.h>
#include <g2logworker.h>

#include <TestRunnerTeamCity.h>

int main(int argc, char *argv[]) {
    g2LogWorker g2log("optimer-tests", "");
    g2::initializeLogging(&g2log);

    return UnitTest::RunAllTestsWithTeamCity();
}
