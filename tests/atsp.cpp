// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <string>
#include <fstream>

#include <UnitTest++.h>
#undef CHECK

#include <tsp/common/data_loader.h>
#include <tsp/asymmetric/tsp.h>

#include "tsp.h"

namespace {
struct Ftv38Fixture {
    Ftv38Fixture() {
        fvt38_solution = 1530;

        size_t dimension;
        std::ifstream ifs("data/ftv38.atsp");
        tsp::load_tsplib_problem(ifs, ftv38_matrix, dimension);
        ifs.close();

        ftv38_instance = new tsp::InitialData(ftv38_matrix, dimension);
    }

    ~Ftv38Fixture() {
        delete ftv38_instance;
    }

    Ftv38Fixture(const Ftv38Fixture &) = delete;
    Ftv38Fixture &operator=(const Ftv38Fixture &) = delete;

    tsp::InitialData *ftv38_instance;
    value_type fvt38_solution;
    std::vector<value_type> ftv38_matrix;
};

SUITE(ATSPTest) {
    TEST_FIXTURE(Ftv38Fixture, test_serial_FTV38_lifo) {
        UNITTEST_TIME_CONSTRAINT(10000);
        tsp_test::test_serial_problem_lifo<TspSolver>
            (*ftv38_instance, fvt38_solution);
    }

    TEST_FIXTURE(Ftv38Fixture, test_serial_FTV38_priority) {
        UNITTEST_TIME_CONSTRAINT(10000);
        tsp_test::test_serial_problem_priority<TspSolver>
            (*ftv38_instance, fvt38_solution);
    }

    TEST_FIXTURE(Ftv38Fixture, test_parallel_FTV38_lifo) {
        UNITTEST_TIME_CONSTRAINT(10000);
        tsp_test::test_parallel_problem_lifo_giving<TspSolver, 4, 2>
            (*ftv38_instance, fvt38_solution);
    }

    TEST_FIXTURE(Ftv38Fixture, test_parallel_FTV38_priority) {
        UNITTEST_TIME_CONSTRAINT(10000);
        tsp_test::test_parallel_problem_priority_giving<TspSolver, 4, 2>
            (*ftv38_instance, fvt38_solution);
    }

    TEST_FIXTURE(Ftv38Fixture, test_parallel_FTV38_lifo_requesting) {
        UNITTEST_TIME_CONSTRAINT(10000);
        tsp_test::test_parallel_problem_lifo_requesting<TspSolver, 4, 2>
            (*ftv38_instance, fvt38_solution);
    }

    TEST_FIXTURE(Ftv38Fixture, test_parallel_FTV38_priority_requesting) {
        UNITTEST_TIME_CONSTRAINT(10000);
        tsp_test::test_parallel_problem_priority_requesting<TspSolver, 4, 2>
            (*ftv38_instance, fvt38_solution);
    }
}
}  // namespace

