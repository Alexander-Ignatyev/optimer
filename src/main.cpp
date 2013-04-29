// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <g2log.h>
#include <g2logworker.h>

#include "tsp.h"
#include "ini_file.h"
#include "parallel_bnb.h"
#include "sequence_bnb.h"
#include "data_loader.h"
#include "giving_scheduler.h"
#include "requesting_scheduler.h"

namespace tsp_config {

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

    std::ostringstream oss;
    solver.print_stats(oss);
    LOG(INFO) << oss.str();
    std::cout << "Found Record: " << record << std::endl;
    std::cout << "Valuation Time: " << valuation_time << std::endl;
}

static const std::string g_general_param = "general";

void get_scheduler_params(const IniSection &scheduler
    , GivingSchedulerParams *params) {
    params->num_threads = std::stoul(
        scheduler["num_threads"], nullptr, 0);
    params->num_minimum_nodes = std::stoul(
        scheduler["num_minimum_nodes"], nullptr, 0);
    params->num_maximum_nodes = std::stoul(
        scheduler["num_maximum_nodes"], nullptr, 0);
}

void get_scheduler_params(const IniSection &scheduler
    , RequestingSchedulerParams *params) {
    params->num_threads = std::stoul(
        scheduler["num_threads"], nullptr, 0);
    params->num_minimum_nodes = std::stoul(
        scheduler["num_minimum_nodes"], nullptr, 0);
}

const std::string &problem_path(const IniFile &ini) {
    return ini[g_general_param]["problem_path"];
}

template <typename Container>
int process_sequence(const IniFile &ini) {
    SequenceBNB<TspSolver, Container > bnb;
    solve(problem_path(ini), bnb);
    return 0;
}

template <typename Container>
int process_parallel_lock(const IniFile &ini) {
    IniSection scheduler = ini["scheduler"];
    std::string scheduler_type = scheduler["type"];
    if (scheduler_type == "giving") {
        GivingSchedulerParams params;
        get_scheduler_params(scheduler, &params);

        GivingScheduler<typename TspSolver::Set> scheduler(params);
        ParallelBNB<TspSolver, Container
            , GivingScheduler<typename TspSolver::Set>>
            bnb(scheduler);
        solve(problem_path(ini), bnb);
    } else if (scheduler_type == "requesting") {
        RequestingSchedulerParams params;
        get_scheduler_params(scheduler, &params);

        RequestingScheduler<typename TspSolver::Set> scheduler(params);
        ParallelBNB<TspSolver, Container
            , RequestingScheduler<typename TspSolver::Set>>
            bnb(scheduler);
        solve(problem_path(ini), bnb);
    } else {
        std::cerr << "Invalid scheduler" << std::endl;
        return 1;
    }
    return 0;
}

template <typename Container>
int process_valuation_type(const IniFile &ini) {
    std::string valuation = ini[g_general_param]["valuation_type"];
    if (valuation == "sequnce") {
        return process_sequence<Container>(ini);
    } else if (valuation == "parallel-lock") {
        return process_parallel_lock<Container>(ini);
    } else {
        std::cerr << "Invalid valuation type" << std::endl;
        return 1;
    }
}

int process_container_type(const IniFile &ini) {
    std::string container = ini[g_general_param]["container_type"];
    if (container == "lifo") {
        return process_valuation_type<LifoContainer>(ini);
    } else if (container == "priority") {
        return process_valuation_type<PriorityContainer>(ini);
    } else {
        std::cerr << "Invalid container type" << std::endl;
        return 1;
    }
}

int solve(std::istream &is) {
    IniFile ini(is);
    return process_container_type(ini);
}
}  // namespace tsp_config

int main(int argc, char *argv[]) {
    g2LogWorker g2log("optimer-atsp", "");
    g2::initializeLogging(&g2log);

    std::string config_path = "config/default.ini";
    if (argc > 1) {
        config_path = argv[1];
    }
    std::ifstream ifs(config_path);
    if (!ifs) {
        std::cerr << "unable to find config file: " << config_path << std::endl;
        return 1;
    }
    return tsp_config::solve(ifs);
}
