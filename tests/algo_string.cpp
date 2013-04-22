// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <cppunit/extensions/HelperMacros.h>

#include <algo_string.h>

class AlgoStringTest: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(AlgoStringTest);
    CPPUNIT_TEST(split_test);
    CPPUNIT_TEST(split_variant2_test);
    CPPUNIT_TEST(ltrim_test);
    CPPUNIT_TEST(rtrim_test);
    CPPUNIT_TEST(trim_test);
    CPPUNIT_TEST_SUITE_END();

    void split_test() {
        std::vector<std::string> parts = split("one:two ::\nfour\n", ':');
        CPPUNIT_ASSERT_EQUAL(parts.size(), static_cast<size_t>(4));
        CPPUNIT_ASSERT_EQUAL(parts[0], std::string("one"));
        CPPUNIT_ASSERT_EQUAL(parts[1], std::string("two "));
        CPPUNIT_ASSERT(parts[2].empty());
        CPPUNIT_ASSERT_EQUAL(parts[3], std::string("\nfour\n"));
    }

    void split_variant2_test() {
        std::vector<std::string> parts;
        split("one:two ::\nfour\n", ':', parts);
        CPPUNIT_ASSERT_EQUAL(parts.size(), static_cast<size_t>(4));
        CPPUNIT_ASSERT_EQUAL(parts[0], std::string("one"));
        CPPUNIT_ASSERT_EQUAL(parts[1], std::string("two "));
        CPPUNIT_ASSERT(parts[2].empty());
        CPPUNIT_ASSERT_EQUAL(parts[3], std::string("\nfour\n"));
    }

    void ltrim_test() {
        std::string str = "hello";
        CPPUNIT_ASSERT_EQUAL(ltrim(str), std::string("hello"));

        str = "hello ";
        CPPUNIT_ASSERT_EQUAL(ltrim(str), std::string("hello "));

        str = "hello \t \n\t";
        CPPUNIT_ASSERT_EQUAL(ltrim(str), std::string("hello \t \n\t"));

        str = " hello";
        CPPUNIT_ASSERT_EQUAL(ltrim(str), std::string("hello"));

        str = " \t \n\t hello";
        CPPUNIT_ASSERT_EQUAL(ltrim(str), std::string("hello"));

        str = " hello ";
        CPPUNIT_ASSERT_EQUAL(ltrim(str), std::string("hello "));

        str = " \t \n\t hello \t \n\t ";
        CPPUNIT_ASSERT_EQUAL(ltrim(str), std::string("hello \t \n\t "));
    }

    void rtrim_test() {
        std::string str = "hello";
        CPPUNIT_ASSERT_EQUAL(rtrim(str), std::string("hello"));

        str = "hello ";
        CPPUNIT_ASSERT_EQUAL(rtrim(str), std::string("hello"));

        str = "hello \t \n\t";
        CPPUNIT_ASSERT_EQUAL(rtrim(str), std::string("hello"));

        str = " hello";
        CPPUNIT_ASSERT_EQUAL(rtrim(str), std::string(" hello"));

        str = " \t \n\t hello";
        CPPUNIT_ASSERT_EQUAL(rtrim(str), std::string(" \t \n\t hello"));

        str = " hello ";
        CPPUNIT_ASSERT_EQUAL(rtrim(str), std::string(" hello"));

        str = " \t \n\t hello \t \n\t ";
        CPPUNIT_ASSERT_EQUAL(rtrim(str), std::string(" \t \n\t hello"));
    }

    void trim_test() {
        std::string str = "hello";
        CPPUNIT_ASSERT_EQUAL(trim(str), std::string("hello"));

        str = "hello ";
        CPPUNIT_ASSERT_EQUAL(trim(str), std::string("hello"));

        str = "hello \t \n\t";
        CPPUNIT_ASSERT_EQUAL(trim(str), std::string("hello"));

        str = " hello";
        CPPUNIT_ASSERT_EQUAL(trim(str), std::string("hello"));

        str = " \t \n\t hello";
        CPPUNIT_ASSERT_EQUAL(trim(str), std::string("hello"));

        str = " hello ";
        CPPUNIT_ASSERT_EQUAL(trim(str), std::string("hello"));

        str = " \t \n\t hello \t \n\t ";
        CPPUNIT_ASSERT_EQUAL(trim(str), std::string("hello"));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AlgoStringTest);
