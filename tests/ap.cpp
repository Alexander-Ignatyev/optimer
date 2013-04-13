#include <algorithm>

#include <cppunit/extensions/HelperMacros.h>

#include <defs.h>
#include <ap_solver.hpp>

void test_problem(const value_type *data, size_t dimension, const std::vector<size_t> &expected_result) {
	AP_Solver<value_type> solver(dimension);
	std::vector<size_t> result = solver.transform(data, dimension);
	CPPUNIT_ASSERT_EQUAL(result.size(), expected_result.size());
	CPPUNIT_ASSERT(std::equal(result.begin(), result.end(), expected_result.begin()));
}

class APTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( APTest );
	CPPUNIT_TEST(testCase1);
	CPPUNIT_TEST(testCase2);
	CPPUNIT_TEST_SUITE_END();
	
	void testCase1() {
		value_type data[] = {1, 2, 3, 6, 5, 4, 1, 1, 1};
		test_problem(data, 3, {0, 2, 1});
	}
	
	void testCase2() {
		value_type data[] = {5, 9, 7, 10, 3, 2, 8, 7, 4};
		test_problem(data, 3, {0, 1, 2});
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( APTest );
