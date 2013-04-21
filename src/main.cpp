// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <iostream>
#include <string>
#include <fstream>

#include "tsp.h"
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

int parallel(const std::string &problem_path) {
    typedef GivingScheduler<typename TspSolver::Set> Scheduler;
    Scheduler scheduler({4, 24, 40});
    ParallelBNB<TspSolver, PriorityContainer, Scheduler> bnb(scheduler);

    solve(problem_path, bnb);

    return 0;
}

int sequnce(const std::string &problem_path) {
    SequenceBNB<TspSolver, PriorityContainer > bnb;

    solve(problem_path, bnb);
    return 0;
}

int main(int argc, char *argv[]) {
    std::cout << "starting..." << std::endl;
    std::string problem_path = "data/ftv47.atsp";
    if (argc > 1) {
        problem_path = argv[1];
    }
    bool is_parallel = true;
    if (is_parallel) {
        return parallel(problem_path);
    } else {
        return sequnce(problem_path);
    }
}
