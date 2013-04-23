// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <iostream>
#include <string>
#include <fstream>

#include "tsp.h"
#include "ini_file.h"
#include "parallel_bnb.h"
#include "sequence_bnb.h"
#include "data_loader.h"
#include "giving_scheduler.h"

template <typename BNBSolver>
void solve(const std::string &problem_path, BNBSolver &solver) {
    value_type *matrix;
    size_t rank;
    std::ifstream ifs(problem_path);
    load_tsplib_problem(ifs, matrix, rank);
    ifs.close();

    TspInitialData data(matrix, rank);

    double valuation_time = -1;
    value_type record = 0;
    try {
        Timer timer;
        record = solver.solve(data).value;
        valuation_time = timer.elapsed_seconds();
    }
    catch(std::bad_alloc &) {
        std::cout << "Out of memory\n";
    }
    delete[] matrix;

    solver.print_stats(std::cout);
    std::cout << "Found Record: " << record << std::endl;
    std::cout << "Valuation Time: " << valuation_time << std::endl;
}

void get_scheduler_params(const IniSection &scheduler
    , GivingSchedulerParams *params) {
    params->num_threads = std::stoul(
        scheduler["num_threads"], nullptr, 0);
    params->num_minimum_nodes = std::stoul(
        scheduler["num_minimum_nodes"], nullptr, 0);
    params->num_maximum_nodes = std::stoul(
        scheduler["num_maximum_nodes"], nullptr, 0);
}

int main(int argc, char *argv[]) {
    static const std::string parallel_lock = "parallel-lock";
    static const std::string sequence = "sequnce";
    static const std::string lifo_container = "lifo";
    static const std::string priority_container = "priority";
    static const std::string giving_scheduler = "giving";

    std::string config_path = "config/default.ini";
    if (argc > 1) {
        config_path = argv[1];
    }
    std::ifstream ifs(config_path);
    if (!ifs) {
        std::cerr << "unable to find config file: " << config_path << std::endl;
        return 1;
    }
    IniFile ini(ifs);
    IniSection general = ini["general"];
    std::string valuation_type = general["valuation_type"];
    std::string container_type = general["container_type"];
    std::string problem_path = general["problem_path"];
    if (valuation_type == parallel_lock) {
        IniSection scheduler = ini["scheduler"];
        std::string scheduler_type = scheduler["type"];

        if (container_type == lifo_container) {
            if (scheduler_type == giving_scheduler) {
                GivingSchedulerParams params;
                get_scheduler_params(scheduler, &params);

                GivingScheduler<typename TspSolver::Set> scheduler(params);
                ParallelBNB<TspSolver, LifoContainer
                    , GivingScheduler<typename TspSolver::Set>>
                    bnb(scheduler);
                solve(problem_path, bnb);
            }
        } else if (container_type == priority_container) {
            if (scheduler_type == giving_scheduler) {
                GivingSchedulerParams params;
                get_scheduler_params(scheduler, &params);

                GivingScheduler<typename TspSolver::Set> scheduler(params);
                ParallelBNB<TspSolver, PriorityContainer
                    , GivingScheduler<typename TspSolver::Set>>
                    bnb(scheduler);
                solve(problem_path, bnb);
            }
        }
    } else if (valuation_type == sequence) {
        if (container_type == lifo_container) {
            SequenceBNB<TspSolver, LifoContainer > bnb;
            solve(problem_path, bnb);
        } else if (container_type == priority_container) {
            SequenceBNB<TspSolver, PriorityContainer > bnb;
            solve(problem_path, bnb);
        }
    }
    return 0;
}
