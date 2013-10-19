// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <iostream>
#include <fstream>
#include <sstream>

#include <g2log.h>
#include <g2logworker.h>

#include <common/ini_file.h>
#include <bnb/serial_bnb.h>
#include <tsp/common/data_loader.h>
#include <tsp/symmetric/lagrangean_stsp.h>

int solve(std::istream &is) {
    IniFile ini(is);
    IniSection general = ini["general"];

    const std::string &problem_path = general["problem_path"];
    std::ifstream ifs(problem_path);
    if (!ifs) {
        std::cerr << "Cannot open problem data file ";
        std::cerr << problem_path << std::endl;
        return 1;
    }

    std::vector<value_type> matrix;
    size_t dimension;

    TspCommon::load_tsplib_problem(ifs, matrix, dimension);
    stsp::LagrangeanSolver::InitialData data(matrix, dimension);

    IniSection stsp_section = ini["stsp"];
    data.parameters = stsp_section.data();

    bnb::SerialBNB<stsp::LagrangeanSolver> solver;
    auto solution = solver.solve(data);
    std::cout << "Solution: " << solution.value << std::endl;
    for (auto point : solution.route) {
        std::cout << point << " ";
    }
    std::ostringstream oss;
    solver.print_stats(oss);
    LOG(INFO) << oss.str();
    std::cout << oss.str();
    return 0;
}

int main(int argc, char *argv[]) {
    g2LogWorker g2log("optimer-stsp", "");
    g2::initializeLogging(&g2log);

    std::string config_path = "config/default_stsp.ini";
    if (argc > 1) {
        config_path = argv[1];
    }
    std::ifstream ifs(config_path);
    if (!ifs) {
        std::cerr << "unable to find config file: " << config_path << std::endl;
        return 1;
    }

    return solve(ifs);
}
