// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <algorithm>

#include <cppunit/extensions/HelperMacros.h>

#include <ap_solver.h>

void test_problem(const unsigned *data, size_t dimension
    , const std::vector<size_t> &expected_result) {
    ApSolver<unsigned> solver;
    std::vector<size_t> result = solver.solve(data, dimension);
    CPPUNIT_ASSERT_EQUAL(result.size(), expected_result.size());
    CPPUNIT_ASSERT(std::equal(result.begin(), result.end()
        , expected_result.begin()));
}

class APTest: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(APTest);
    CPPUNIT_TEST(testCase1);
    CPPUNIT_TEST(testCase2);
    CPPUNIT_TEST(testCase3);
    CPPUNIT_TEST_SUITE_END();

    void testCase1() {
        unsigned data[] = {1, 2, 3, 6, 5, 4, 1, 1, 1};
        test_problem(data, 3, {0, 2, 1});
    }

    void testCase2() {
        unsigned data[] = {5, 9, 7, 10, 3, 2, 8, 7, 4};
        test_problem(data, 3, {0, 1, 2});
    }


    void testCase3() {
        unsigned data[] =  {  7, 8, 8, 9
                            , 2, 8, 5, 7
                            , 1, 6, 6, 9
                            , 3, 6, 2, 2};
        test_problem(data, 4, {1, 2, 0, 3});
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(APTest);
