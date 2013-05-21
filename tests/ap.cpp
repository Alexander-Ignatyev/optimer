// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <algorithm>

#include <cppunit/extensions/HelperMacros.h>

#include <ap_solver.h>

void test_problem(const double *data, size_t dimension
    , const std::vector<size_t> &expected_result
    , const std::vector<size_t> &partial_solution = {}) {
    ApSolver<double> solver;
    ApSolver<double>::Solution ap_solution_new;
    if (!partial_solution.empty()) {
        ap_solution_new.init(dimension);
        ap_solution_new.primal = partial_solution;
    }
    solver.solve(data, dimension, &ap_solution_new);
    const std::vector<size_t> &result = ap_solution_new.primal;
    CPPUNIT_ASSERT_EQUAL(result.size(), expected_result.size());
    CPPUNIT_ASSERT(std::equal(result.begin(), result.end()
            , expected_result.begin()));
}

class APTest: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(APTest);
    CPPUNIT_TEST(testCase1);
    CPPUNIT_TEST(testCase2);
    CPPUNIT_TEST(testCase3);
    CPPUNIT_TEST(testCase3_partial_solution);
    CPPUNIT_TEST_SUITE_END();

    void testCase1() {
        double data[] = {1, 2, 3, 6, 5, 4, 1, 1, 1};
        test_problem(data, 3, {0, 2, 1});
    }

    void testCase2() {
        double data[] = {5, 9, 7, 10, 3, 2, 8, 7, 4};
        test_problem(data, 3, {0, 1, 2});
    }

    void testCase3() {
        double data[] =  {  7, 8, 8, 9
                            , 2, 8, 5, 7
                            , 1, 6, 6, 9
                            , 3, 6, 2, 2};
        test_problem(data, 4, {1, 2, 0, 3});
    }

    void testCase3_partial_solution() {
        double data[] =  {  7, 8, 8, 9
                            , 2, 8, 5, 7
                            , 1, 6, 6, 9
                            , 3, 6, 2, 2};
        test_problem(data, 4, {1, 2, 0, 3}, {1, 4, 0, 3});
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(APTest);
