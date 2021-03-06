// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#include <algorithm>
#include <UnitTest++.h>
#include <tsp/asymmetric/ap_solver.h>

namespace {
void test_problem(const double *data, size_t dimension
    , const std::vector<size_t> &expected_result
    , const std::vector<size_t> &partial_solution = std::vector<size_t>()) {
    ApSolver<double> solver;
    ApSolver<double>::Solution ap_solution_new;
    if (!partial_solution.empty()) {
        ap_solution_new.init(dimension);
        ap_solution_new.primal = partial_solution;
    }
    solver.solve(data, dimension, &ap_solution_new);
    const std::vector<size_t> &result = ap_solution_new.primal;
    CHECK_EQUAL(result.size(), expected_result.size());
    CHECK(std::equal(result.begin(), result.end()
            , expected_result.begin()));
}

SUITE(APTest) {
    TEST(testCase1) {
        double data[] = {1, 2, 3, 6, 5, 4, 1, 1, 1};
        size_t array[] = {0, 2, 1};
        std::vector<size_t> expected_result(array, array+3);
        test_problem(data, 3, expected_result);
    }

    TEST(testCase2) {
        double data[] = {5, 9, 7, 10, 3, 2, 8, 7, 4};
        size_t array[] = {0, 1, 2};
        std::vector<size_t> expected_result(array, array+3);
        test_problem(data, 3, expected_result);
    }

    TEST(testCase3) {
        double data[] =  {  7, 8, 8, 9
                            , 2, 8, 5, 7
                            , 1, 6, 6, 9
                            , 3, 6, 2, 2};
        size_t array[] = {1, 2, 0, 3};
        std::vector<size_t> expected_result(array, array+4);
        test_problem(data, 4, expected_result);
    }

    TEST(testCase3_partial_solution) {
        double data[] =  {  7, 8, 8, 9
                            , 2, 8, 5, 7
                            , 1, 6, 6, 9
                            , 3, 6, 2, 2};
        size_t array_er[] = {1, 2, 0, 3};
        std::vector<size_t> expected_result(array_er, array_er+4);
        size_t array_ps[] = {1, 4, 0, 3};
        std::vector<size_t> partial_solution(array_ps, array_ps+4);
        test_problem(data, 4, expected_result, partial_solution);
    }
}
}  // namespace
