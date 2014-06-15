// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#include <string>
#include <fstream>

#include <UnitTest++.h>
#undef CHECK

#include <bnb/simple_bnb.h>
#include <tsp/common/data_loader.h>
#include <tsp/asymmetric/tsp.h>

namespace {
struct Lmsc1963Fixture {
    Lmsc1963Fixture() {
        solution = 63;
        size_t dimension;
        std::ifstream ifs("data/lmsc1963.atsp");
        tsp::load_tsplib_problem(ifs, matrix, dimension);
        ifs.close();
        instance = new tsp::InitialData(matrix, dimension);
    }

    ~Lmsc1963Fixture() {
        delete instance;
    }

    tsp::InitialData *instance;
    value_type solution;
    std::vector<value_type> matrix;

 private:
    Lmsc1963Fixture(const Lmsc1963Fixture &);
    Lmsc1963Fixture &operator=(const Lmsc1963Fixture &);
};

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

    tsp::InitialData *ftv38_instance;
    value_type fvt38_solution;
    std::vector<value_type> ftv38_matrix;

 private:
    Ftv38Fixture(const Ftv38Fixture &);
    Ftv38Fixture &operator=(const Ftv38Fixture &);
};

SUITE(SimpleATSPTest) {
    TEST_FIXTURE(Ftv38Fixture, test_simple_FTV38) {
        UNITTEST_TIME_CONSTRAINT(10000);
        bnb::SimpleBNB<TspSolver> solver;
        value_type record = solver.solve(*ftv38_instance).value;
        CHECK_CLOSE(record, fvt38_solution, 0.001);
    }

    TEST_FIXTURE(Lmsc1963Fixture, test_simple_Lmsc1963_br1) {
        UNITTEST_TIME_CONSTRAINT(10000);
        bnb::SimpleBNB<TspSolver> solver;
        instance->parameters["branching_rule"] = "1";
        value_type record = solver.solve(*instance).value;
        CHECK_CLOSE(record, solution, 0.001);
    }

    TEST_FIXTURE(Lmsc1963Fixture, test_simple_Lmsc1963_br2) {
        UNITTEST_TIME_CONSTRAINT(10000);
        bnb::SimpleBNB<TspSolver> solver;
        instance->parameters["branching_rule"] = "2";
        value_type record = solver.solve(*instance).value;
        CHECK_CLOSE(record, solution, 0.001);
    }

    TEST_FIXTURE(Lmsc1963Fixture, test_simple_Lmsc1963_br3) {
        UNITTEST_TIME_CONSTRAINT(10000);
        bnb::SimpleBNB<TspSolver> solver;
        instance->parameters["branching_rule"] = "3";
        value_type record = solver.solve(*instance).value;
        CHECK_CLOSE(record, solution, 0.001);
    }

    TEST_FIXTURE(Lmsc1963Fixture, test_simple_Lmsc1963_br4) {
        UNITTEST_TIME_CONSTRAINT(10000);
        bnb::SimpleBNB<TspSolver> solver;
        instance->parameters["branching_rule"] = "4";
        value_type record = solver.solve(*instance).value;
        CHECK_CLOSE(record, solution, 0.001);
    }

    TEST_FIXTURE(Lmsc1963Fixture, test_simple_Lmsc1963_br5) {
        UNITTEST_TIME_CONSTRAINT(10000);
        bnb::SimpleBNB<TspSolver> solver;
        instance->parameters["branching_rule"] = "5";
        value_type record = solver.solve(*instance).value;
        CHECK_CLOSE(record, solution, 0.001);
    }
}
}  // namespace
