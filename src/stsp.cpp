// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <iostream>
#include <fstream>
#include <string>

#include <common/log.h>
#include <tsp/common/config.h>
#include <tsp/symmetric/lagrangean_stsp.h>

int main(int argc, char *argv[]) {
    init_logger("optimer-stsp");

    std::string config_path = "config/default_stsp.ini";
    if (argc > 1) {
        config_path = argv[1];
    }
    std::ifstream ifs(config_path);
    if (!ifs) {
        std::cerr << "unable to find config file: " << config_path << std::endl;
        return 1;
    }

    return tsp::solve<stsp::LagrangeanSolver>(ifs);
}
