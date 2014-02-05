// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <string>
#include <fstream>

#include <UnitTest++.h>
#undef CHECK

#include <bnb/simple_bnb.h>
#include <tsp/common/data_loader.h>
#include <tsp/asymmetric/tsp.h>

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

    tsp::InitialData *ftv38_instance;
    value_type fvt38_solution;
    std::vector<value_type> ftv38_matrix;

 private:
    Ftv38Fixture(const Ftv38Fixture &);
    Ftv38Fixture &operator=(const Ftv38Fixture &);
};

SUITE(ATSPTest) {
    TEST_FIXTURE(Ftv38Fixture, test_simple_FTV38) {
        UNITTEST_TIME_CONSTRAINT(10000);
        bnb::SimpleBNB<TspSolver> solver;
        value_type record = solver.solve(*ftv38_instance).value;
        CHECK_CLOSE(record, fvt38_solution, 0.001);
    }
}
}  // namespace

