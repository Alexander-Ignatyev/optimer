// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_COMMON_CONFIG_H_
#define TSP_COMMON_CONFIG_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <common/ini_file.h>
#include <bnb/parallel_bnb.h>
#include <bnb/serial_bnb.h>
#include <bnb/giving_scheduler.h>
#include <bnb/requesting_scheduler.h>

#include <tsp/common/types.h>
#include <tsp/common/data_loader.h>

namespace tsp {
static const std::string g_general_param = "general";
const std::string &problem_path(const IniFile &ini) {
    return ini[g_general_param]["problem_path"];
}

template <typename BNBSolver>
void solve(const IniFile &ini, BNBSolver &solver) {
    size_t rank;
    std::ifstream ifs(problem_path(ini));
    std::vector<value_type> matrix;
    tsp::load_tsplib_problem(ifs, matrix, rank);
    ifs.close();

    tsp::InitialData data(matrix, rank);
    data.parameters = ini["tsp"].data();

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

    std::ostringstream oss;
    solver.print_stats(oss);
    LOG(INFO) << oss.str();
    std::cout << "Found Record: " << record << std::endl;
    std::cout << "Valuation Time: " << valuation_time << std::endl;
}

void get_scheduler_params(const IniSection &scheduler
                          , bnb::GivingSchedulerParams *params) {
    params->num_threads = std::stoul(
                            scheduler["num_threads"], nullptr, 0);
    params->num_minimum_nodes = std::stoul(
                            scheduler["num_minimum_nodes"], nullptr, 0);
    params->num_maximum_nodes = std::stoul(
                            scheduler["num_maximum_nodes"], nullptr, 0);
}

void get_scheduler_params(const IniSection &scheduler
                          , bnb::RequestingSchedulerParams *params) {
    params->num_threads = std::stoul(
                            scheduler["num_threads"], nullptr, 0);
    params->num_minimum_nodes = std::stoul(
                            scheduler["num_minimum_nodes"], nullptr, 0);

    if (params->num_threads == 0) {
        std::cerr << "invalid num_threads param" << std::endl;
    }
    if (params->num_minimum_nodes == 0) {
        std::cerr << "invalid num_minimum_nodes param" << std::endl;
    }
}

template <typename Solver, typename Container>
int process_serial(const IniFile &ini) {
    bnb::SerialBNB<Solver, Container > bnb;
    solve(ini, bnb);
    return 0;
}

template <typename Solver, typename Container>
int process_parallel_lock(const IniFile &ini) {
    IniSection scheduler = ini["scheduler"];
    std::string scheduler_type = scheduler["type"];
    if (scheduler_type == "giving") {
        bnb::GivingSchedulerParams params;
        get_scheduler_params(scheduler, &params);

        bnb::GivingScheduler<typename Solver::Set> scheduler(params);
        bnb::ParallelBNB<Solver, Container
        , bnb::GivingScheduler<typename Solver::Set>>
        bnb(scheduler);
        solve(ini, bnb);
    } else if (scheduler_type == "requesting") {
        bnb::RequestingSchedulerParams params;
        get_scheduler_params(scheduler, &params);

        bnb::RequestingScheduler<typename Solver::Set> scheduler(params);
        bnb::ParallelBNB<Solver, Container
        , bnb::RequestingScheduler<typename Solver::Set>>
        bnb(scheduler);
        solve(ini, bnb);
    } else {
        std::cerr << "Invalid scheduler" << std::endl;
        return 1;
    }
    return 0;
}

template <typename Solver, typename Container>
int process_valuation_type(const IniFile &ini) {
    std::string valuation = ini[g_general_param]["valuation_type"];
    if (valuation == "serial") {
        return process_serial<Solver, Container>(ini);
    } else if (valuation == "parallel-lock") {
        return process_parallel_lock<Solver, Container>(ini);
    } else {
        std::cerr << "Invalid valuation type" << std::endl;
        return 1;
    }
}

template <typename Solver>
int process_container_type(const IniFile &ini) {
    std::string container = ini[g_general_param]["container_type"];
    if (container == "lifo") {
        return process_valuation_type<Solver, bnb::LifoContainer>(ini);
    } else if (container == "priority") {
        return process_valuation_type<Solver, bnb::PriorityContainer>(ini);
    } else {
        std::cerr << "Invalid container type: " << container << std::endl;
        return 1;
    }
}

template <typename Solver>
int solve(std::istream &is) {
    IniFile ini(is);
    return process_container_type<Solver>(ini);
}

}  // namespace tsp

#endif  // TSP_COMMON_CONFIG_H_
