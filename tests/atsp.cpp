// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <string>
#include <fstream>

#include <UnitTest++.h>
#undef CHECK

#include <bnb/parallel_bnb.h>
#include <bnb/serial_bnb.h>
#include <bnb/giving_scheduler.h>
#include <bnb/requesting_scheduler.h>

#include <tsp/asymmetric/data_loader.h>
#include <tsp/asymmetric/tsp.h>

namespace {
template <typename Solver>
void test_problem(Solver &solver, const TspInitialData &data
    , value_type expected_value) {
    value_type record = solver.solve(data).value;
    CHECK_EQUAL(record, expected_value);
}

void test_serial_problem_lifo(const TspInitialData &data
    , value_type expected_value) {
    SerialBNB<TspSolver, LifoContainer > solver;
    test_problem(solver, data, expected_value);
}

void test_serial_problem_priority(const TspInitialData &data
    , value_type expected_value) {
    SerialBNB<TspSolver, PriorityContainer> solver;
    test_problem(solver, data, expected_value);
}

void test_parallel_problem_lifo_giving(const TspInitialData &data
    , value_type expected_value) {
    typedef GivingScheduler<typename TspSolver::Set> Scheduler;
    Scheduler scheduler({4, 8, static_cast<unsigned>(data.rank*2)});
    ParallelBNB<TspSolver, LifoContainer, Scheduler > solver(scheduler);
    test_problem(solver, data, expected_value);
}

void test_parallel_problem_priority_giving(const TspInitialData &data
    , value_type expected_value) {
    typedef GivingScheduler<typename TspSolver::Set> Scheduler;
    Scheduler scheduler({4, 8, static_cast<unsigned>(data.rank*2)});
    ParallelBNB<TspSolver, PriorityContainer, Scheduler>
        solver(scheduler);
    test_problem(solver, data, expected_value);
}

void test_parallel_problem_lifo_requesting(const TspInitialData &data
    , value_type expected_value) {
    typedef RequestingScheduler<typename TspSolver::Set> Scheduler;
    Scheduler scheduler({4, 8});
    ParallelBNB<TspSolver, LifoContainer, Scheduler > solver(scheduler);
    test_problem(solver, data, expected_value);
}

void test_parallel_problem_priority_requesting(const TspInitialData &data
    , value_type expected_value) {
    typedef RequestingScheduler<typename TspSolver::Set> Scheduler;
    Scheduler scheduler({4, 8});
    ParallelBNB<TspSolver, PriorityContainer, Scheduler>
        solver(scheduler);
    test_problem(solver, data, expected_value);
}


struct Ftv38Fixture {
    Ftv38Fixture() {
        fvt38_solution = 1530;

        size_t dimension;
        std::ifstream ifs("data/ftv38.atsp");
        load_tsplib_problem(ifs, ftv38_matrix, dimension);
        ifs.close();

        ftv38_instance = new TspInitialData(ftv38_matrix, dimension);
    }

    ~Ftv38Fixture() {
        delete [] ftv38_matrix;
        delete ftv38_instance;
    }

    Ftv38Fixture(const Ftv38Fixture &) = delete;
    Ftv38Fixture &operator=(const Ftv38Fixture &) = delete;

    TspInitialData *ftv38_instance;
    value_type *ftv38_matrix;
    value_type fvt38_solution;
};

SUITE(ATSPTest) {
    TEST_FIXTURE(Ftv38Fixture, test_serial_FTV38_lifo) {
        UNITTEST_TIME_CONSTRAINT(10000);
        test_serial_problem_lifo(*ftv38_instance, fvt38_solution);
    }

    TEST_FIXTURE(Ftv38Fixture, test_serial_FTV38_priority) {
        UNITTEST_TIME_CONSTRAINT(10000);
        test_serial_problem_priority(*ftv38_instance, fvt38_solution);
    }

    TEST_FIXTURE(Ftv38Fixture, test_parallel_FTV38_lifo) {
        UNITTEST_TIME_CONSTRAINT(10000);
        test_parallel_problem_lifo_giving(*ftv38_instance, fvt38_solution);
    }

    TEST_FIXTURE(Ftv38Fixture, test_parallel_FTV38_priority) {
        UNITTEST_TIME_CONSTRAINT(10000);
        test_parallel_problem_priority_giving(*ftv38_instance, fvt38_solution);
    }

    TEST_FIXTURE(Ftv38Fixture, test_parallel_FTV38_lifo_requesting) {
        UNITTEST_TIME_CONSTRAINT(10000);
        test_parallel_problem_lifo_requesting(*ftv38_instance, fvt38_solution);
    }

    TEST_FIXTURE(Ftv38Fixture, test_parallel_FTV38_priority_requesting) {
        UNITTEST_TIME_CONSTRAINT(10000);
        test_parallel_problem_priority_requesting
            (*ftv38_instance, fvt38_solution);
    }
}
}  // namespace

