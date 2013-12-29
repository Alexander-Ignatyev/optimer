// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <common/log.h>
#include <TestRunnerTeamCity.h>

int main(int argc, char *argv[]) {
    init_logger("optimer-tests");

    return UnitTest::RunAllTestsWithTeamCity();
}
