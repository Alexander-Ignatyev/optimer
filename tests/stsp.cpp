// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <vector>

#include <UnitTest++.h>
#undef CHECK

#include <bnb/serial_bnb.h>
#include <tsp/symmetric/stsp.h>

using bnb::LifoContainer;
using bnb::PriorityContainer;

using bnb::SerialBNB;

namespace ClassicalSolver {

typedef stsp::ClassicalSolver Solver;
typedef Solver::InitialData InitialData;

template <typename ProblemSolver>
void test_problem(ProblemSolver &solver
                  , const InitialData &data
                  , value_type expected_value) {
    value_type record = solver.solve(data).value;
    CHECK_EQUAL(record, expected_value);
}

void test_serial_problem_lifo(const InitialData &data
                              , value_type expected_value) {
    SerialBNB<Solver, LifoContainer > solver;
    test_problem(solver, data, expected_value);
}

void test_serial_problem_priority(const InitialData &data
                                  , value_type expected_value) {
    SerialBNB<Solver, PriorityContainer> solver;
    test_problem(solver, data, expected_value);
}


struct Simple6Fuxture {
    Simple6Fuxture()
        : initial_data(nullptr)
        , solution(11) {
        matrix =  {1000, 2, 2, 3, 3, 3,
                    2, 1000, 1, 1, 1, 3,
                    2, 1, 1000, 3, 3, 3,
                    3, 1, 3, 1000, 3, 3,
                    3, 1, 3, 3, 1000, 1,
                    3, 3, 3, 3, 1, 1000};
        initial_data = new InitialData(matrix.data(), 6);
    }

    ~Simple6Fuxture() {
        delete initial_data;
    }

    Simple6Fuxture(const Simple6Fuxture &) = delete;
    Simple6Fuxture &operator=(Simple6Fuxture &) = delete;

    std::vector<value_type> matrix;
    InitialData *initial_data;
    value_type solution;
};

SUITE(STSP_TEST) {
    TEST_FIXTURE(Simple6Fuxture, test_serial_Simple6_lifo) {
        UNITTEST_TIME_CONSTRAINT(10000);
        test_serial_problem_lifo(*initial_data, solution);
    }

    TEST_FIXTURE(Simple6Fuxture, test_serial_Simple6_priority) {
        UNITTEST_TIME_CONSTRAINT(10000);
        test_serial_problem_priority(*initial_data, solution);
    }
}
}  // namespace ClassicalSolver
