// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <string>

#include <cppunit/extensions/HelperMacros.h>

#include <parallel_bnb.h>
#include <serial_bnb.h>
#include <tsp.h>
#include <data_loader.h>
#include <giving_scheduler.h>
#include <requesting_scheduler.h>

template <typename Solver>
void test_problem(Solver &solver, const TspInitialData &data
    , value_type expected_value) {
    value_type record = solver.solve(data).value;
    CPPUNIT_ASSERT_EQUAL(record, expected_value);
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


class ATSPTest: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ATSPTest);
    CPPUNIT_TEST(test_serial_FTV38_lifo);
    CPPUNIT_TEST(test_serial_FTV38_priority);
    CPPUNIT_TEST(test_parallel_FTV38_lifo);
    CPPUNIT_TEST(test_parallel_FTV38_priority);
    CPPUNIT_TEST(test_parallel_FTV38_lifo_requesting);
    CPPUNIT_TEST(test_parallel_FTV38_priority_requesting);
    CPPUNIT_TEST_SUITE_END();

    void test_serial_FTV38_lifo() {
        test_serial_problem_lifo(*ftv38_instance, fvt38_solution);
    }

    void test_serial_FTV38_priority() {
        test_serial_problem_priority(*ftv38_instance, fvt38_solution);
    }

    void test_parallel_FTV38_lifo() {
        test_parallel_problem_lifo_giving(*ftv38_instance, fvt38_solution);
    }

    void test_parallel_FTV38_priority() {
        test_parallel_problem_priority_giving(*ftv38_instance, fvt38_solution);
    }

    void test_parallel_FTV38_lifo_requesting() {
        test_parallel_problem_lifo_requesting(*ftv38_instance, fvt38_solution);
    }

    void test_parallel_FTV38_priority_requesting() {
        test_parallel_problem_priority_requesting
            (*ftv38_instance, fvt38_solution);
    }

    TspInitialData *ftv38_instance;
    value_type *ftv38_matrix;
    value_type fvt38_solution;

 public:
    ATSPTest()
        : ftv38_instance(nullptr)
        , ftv38_matrix(nullptr)
        , fvt38_solution(0) {}

    void setUp() override {
        fvt38_solution = 1530;

        size_t dimension;
        std::ifstream ifs("data/ftv38.atsp");
        load_tsplib_problem(ifs, ftv38_matrix, dimension);
        ifs.close();

        ftv38_instance = new TspInitialData(ftv38_matrix, dimension);
    }

    void tearDown() override {
        delete [] ftv38_matrix;
        delete ftv38_instance;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ATSPTest);
