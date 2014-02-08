// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <common/log.h>
#include <common/unix_timer.h>
#include <bnb/simple_bnb.h>
#include <tsp/common/data_loader.h>
#include <tsp/symmetric/lagrangean_stsp.cpp>

int main(int argc, char *argv[]) {
    init_logger("optimer-simple-tsp");

    std::string filename = "data/stsp/gr24.tsp";
    std::ifstream is(filename.c_str());
    if (!is) {
        std::cerr << "Data file: " << filename << " not dound!" << std::endl;
        return 1;
    }
    std::vector<value_type> matrix;
    size_t dimension;

    tsp::load_tsplib_problem(is, matrix, dimension);
    tsp::InitialData data(matrix, dimension);
    data.parameters["epsilon"] = "0.9";
    data.parameters["gradinet_max_iters"] = "100";
    data.parameters["branching_rule"] = "2";

    bnb::SimpleBNB<stsp::LagrangeanSolver> solver;

    double valuation_time = 0;
    value_type record = 0;
    try {
        UnixTimer timer;
        record = solver.solve(data).value;
        valuation_time = timer.elapsed_seconds();
    }
    catch(std::bad_alloc &) {
        std::cerr << "Out of memory\n";
    }

    std::ostringstream oss;
    solver.print_stats(oss);
    LOG(INFO) << oss.str();
    std::cout << "Found Record: " << record << std::endl;
    std::cout << "Valuation Time: " << valuation_time << std::endl;
}
