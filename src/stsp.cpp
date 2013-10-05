// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <iostream>
#include <fstream>
#include <sstream>

#include <g2log.h>
#include <g2logworker.h>

#include <bnb/serial_bnb.h>
#include <tsp/common/data_loader.h>
#include <tsp/symmetric/lagrangean_stsp.h>

int main(int argc, char *argv[]) {
    g2LogWorker g2log("optimer-stsp", "");
    g2::initializeLogging(&g2log);

    std::ifstream ifs("data/stsp/gr24.tsp");
    if (ifs) {
        std::vector<value_type> matrix;
        size_t dimension;
        TspCommon::load_tsplib_problem(ifs, matrix, dimension);
        stsp::LagrangeanSolver::InitialData data(matrix, dimension);
        SerialBNB<stsp::LagrangeanSolver> solver;
        auto solution = solver.solve(data);
        std::cout << "Solution: " << solution.value << std::endl;
        for (auto point : solution.route) {
            std::cout << point << " ";
        }
        std::ostringstream oss;
        solver.print_stats(oss);
        LOG(INFO) << oss.str();
        std::cout << oss.str();
    }
    return 0;
}
