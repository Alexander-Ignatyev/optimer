// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <g2log.h>
#include <g2logworker.h>

#include <bnb/serial_bnb.h>
#include <tsp/symmetric/stsp.h>


#include <iostream>

int main(int argc, char *argv[]) {
    g2LogWorker g2log("optimer-stsp", "");
    g2::initializeLogging(&g2log);

    value_type matrix[] =  {1000, 2, 2, 3, 3, 3,
                     2, 1000, 1, 1, 1, 3,
                     2, 1, 1000, 3, 3, 3,
                     3, 1, 3, 1000, 3, 3,
                     3, 1, 3, 3, 1000, 1,
                     3, 3, 3, 3, 1, 1000};
    size_t size = 6;

    stsp::ClassicalSolver::InitialData data(matrix, size);

    SerialBNB<stsp::ClassicalSolver> solver;
    auto solution = solver.solve(data);
    std::cout << "Solution: " << solution.value << std::endl;
    for (auto point : solution.route) {
        std::cout << point << " ";
    }
    std::cout << std::endl;
    return 0;
}
