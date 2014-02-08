// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#include <string>
#include <fstream>

#include <UnitTest++.h>
#undef CHECK

#include <bnb/simple_bnb.h>
#include <tsp/common/data_loader.h>
#include <tsp/symmetric/lagrangean_stsp.h>

namespace {
struct Gr17Fixture {
    Gr17Fixture()
    : dimension(0)
    , solution_value(2085) {
        size_t dimension;
        std::ifstream ifs("data/stsp/gr17.tsp");
        tsp::load_tsplib_problem(ifs, matrix, dimension);
        ifs.close();
        initial_data = new tsp::InitialData(matrix, dimension);
    }
    ~Gr17Fixture() {
        delete initial_data;
    }
    std::vector<value_type> matrix;
    size_t dimension;
    tsp::InitialData *initial_data;
    value_type solution_value;

 private:
    Gr17Fixture(const Gr17Fixture &);
    Gr17Fixture &operator=(Gr17Fixture &);
};

struct Gr24Fixture {
    Gr24Fixture()
    : dimension(0)
    , solution_value(1272) {
        size_t dimension;
        std::ifstream ifs("data/stsp/gr24.tsp");
        tsp::load_tsplib_problem(ifs, matrix, dimension);
        ifs.close();
        initial_data = new tsp::InitialData(matrix, dimension);
        initial_data->parameters["epsilon"] = "0.9";
        initial_data->parameters["gradinet_max_iters"] = "100";
        initial_data->parameters["branching_rule"] = "2";
    }
    ~Gr24Fixture() {
        delete initial_data;
    }

    std::vector<value_type> matrix;
    size_t dimension;
    tsp::InitialData *initial_data;
    value_type solution_value;
 private:
    Gr24Fixture(const Gr24Fixture &);
    Gr24Fixture &operator=(Gr24Fixture &);
};

SUITE(SimpleSTSPTest) {
    TEST_FIXTURE(Gr17Fixture, test_simple_STSP17) {
        UNITTEST_TIME_CONSTRAINT(10000);
        bnb::SimpleBNB<stsp::LagrangeanSolver> solver;
        value_type record = solver.solve(*initial_data).value;
        CHECK_CLOSE(record, solution_value, 0.001);
    }
    TEST_FIXTURE(Gr24Fixture, test_simple_STSP24) {
        UNITTEST_TIME_CONSTRAINT(10000);
        bnb::SimpleBNB<stsp::LagrangeanSolver> solver;
        value_type record = solver.solve(*initial_data).value;
        CHECK_CLOSE(record, solution_value, 0.001);
    }
}
}  // namespace
