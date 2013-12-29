// Copyright (c) 2008-2013 Alexander Ignatyev. All rights reserved.

#include <iostream>
#include <fstream>
#include <vector>

#include <bnb/defs.h>
#include <common/log.h>
#include <common/ini_file.h>
#include <common/timer.h>
#include <tsp/common/data_loader.h>
#include <heuristics/aco/tsp/aco.h>

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

    tsp::load_tsplib_problem(ifs, matrix, dimension);

    tsp::InitialData data(matrix, dimension);
    data.parameters = ini["aco"].data();
    Timer timer;
    auto solution = aco_tsp::solve(data);
    std::cout << "Time: " << timer.elapsed_seconds() << std::endl;
    std::cout << "Solution: " << solution.value << std::endl;
    solution.write_as_json(std::cout);
    std::cout << std::endl;
    return 0;
}

int main(int argc, char *argv[]) {
    init_logger("aco-tsp");

    std::string config_path = "config/default.ini";
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
