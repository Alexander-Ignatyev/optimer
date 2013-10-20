// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_H_
#define TSP_H_

#include <bnb/parallel_bnb.h>
#include <bnb/serial_bnb.h>
#include <bnb/giving_scheduler.h>
#include <bnb/requesting_scheduler.h>

namespace tsp_test {

using bnb::LifoContainer;
using bnb::PriorityContainer;

using bnb::GivingScheduler;
using bnb::RequestingScheduler;

using bnb::SerialBNB;
using bnb::ParallelBNB;

template <typename Solver>
void test_problem(Solver &solver, const tsp::InitialData &data
                  , value_type expected_value) {
    value_type record = solver.solve(data).value;
    CHECK_CLOSE(record, expected_value, 0.001);
}

template <typename ProblemSolver>
void test_serial_problem_lifo(const tsp::InitialData &data
                              , value_type expected_value) {
    SerialBNB<ProblemSolver, LifoContainer > solver;
    test_problem(solver, data, expected_value);
}

template <typename ProblemSolver>
void test_serial_problem_priority(const tsp::InitialData &data
                                  , value_type expected_value) {
    SerialBNB<ProblemSolver, PriorityContainer> solver;
    test_problem(solver, data, expected_value);
}

template <typename ProblemSolver, int num_threads, int num_minimum_nodes>
void test_parallel_problem_lifo_giving(const tsp::InitialData &data
                                       , value_type expected_value) {
    typedef GivingScheduler<typename ProblemSolver::Set> Scheduler;
    Scheduler scheduler({
          num_threads
        , num_minimum_nodes
        , static_cast<unsigned>(data.rank*2)});
    ParallelBNB<ProblemSolver, LifoContainer, Scheduler > solver(scheduler);
    test_problem(solver, data, expected_value);
}

template <typename ProblemSolver, int num_threads, int num_minimum_nodes>
void test_parallel_problem_priority_giving(const tsp::InitialData &data
                                           , value_type expected_value) {
    typedef GivingScheduler<typename ProblemSolver::Set> Scheduler;
    Scheduler scheduler({
        num_threads
        , num_minimum_nodes
        , static_cast<unsigned>(data.rank*2)});
    ParallelBNB<ProblemSolver, PriorityContainer, Scheduler>
    solver(scheduler);
    test_problem(solver, data, expected_value);
}

template <typename ProblemSolver, int num_threads, int num_minimum_nodes>
void test_parallel_problem_lifo_requesting(const tsp::InitialData &data
                                           , value_type expected_value) {
    typedef RequestingScheduler<typename ProblemSolver::Set> Scheduler;
    Scheduler scheduler({num_threads, num_minimum_nodes});
    ParallelBNB<ProblemSolver, LifoContainer, Scheduler > solver(scheduler);
    test_problem(solver, data, expected_value);
}

template <typename ProblemSolver, int num_threads, int num_minimum_nodes>
void test_parallel_problem_priority_requesting(const tsp::InitialData &data
                                               , value_type expected_value) {
    typedef RequestingScheduler<typename ProblemSolver::Set> Scheduler;
    Scheduler scheduler({num_threads, num_minimum_nodes});
    ParallelBNB<ProblemSolver, PriorityContainer, Scheduler>
    solver(scheduler);
    test_problem(solver, data, expected_value);
}

}  // namespace tsp_test
#endif  // TSP_H_
