// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <UnitTest++.h>
#include <common/algo_string.h>

namespace {
SUITE(AlgoStringTest) {
    TEST(split_test) {
        std::vector<std::string> parts = split("one:two ::\nfour\n", ':');
        CHECK_EQUAL(parts.size(), static_cast<size_t>(4));
        CHECK_EQUAL(parts[0], std::string("one"));
        CHECK_EQUAL(parts[1], std::string("two "));
        CHECK(parts[2].empty());
        CHECK_EQUAL(parts[3], std::string("\nfour\n"));
    }

    TEST(split_variant2_test) {
        std::vector<std::string> parts;
        split("one:two ::\nfour\n", ':', parts);
        CHECK_EQUAL(parts.size(), static_cast<size_t>(4));
        CHECK_EQUAL(parts[0], std::string("one"));
        CHECK_EQUAL(parts[1], std::string("two "));
        CHECK(parts[2].empty());
        CHECK_EQUAL(parts[3], std::string("\nfour\n"));
    }

    TEST(tltrim_test) {
        std::string str = "hello";
        CHECK_EQUAL(ltrim(str), std::string("hello"));

        str = "hello ";
        CHECK_EQUAL(ltrim(str), std::string("hello "));

        str = "hello \t \n\t";
        CHECK_EQUAL(ltrim(str), std::string("hello \t \n\t"));

        str = " hello";
        CHECK_EQUAL(ltrim(str), std::string("hello"));

        str = " \t \n\t hello";
        CHECK_EQUAL(ltrim(str), std::string("hello"));

        str = " hello ";
        CHECK_EQUAL(ltrim(str), std::string("hello "));

        str = " \t \n\t hello \t \n\t ";
        CHECK_EQUAL(ltrim(str), std::string("hello \t \n\t "));
    }

    TEST(rtrim_test) {
        std::string str = "hello";
        CHECK_EQUAL(rtrim(str), std::string("hello"));

        str = "hello ";
        CHECK_EQUAL(rtrim(str), std::string("hello"));

        str = "hello \t \n\t";
        CHECK_EQUAL(rtrim(str), std::string("hello"));

        str = " hello";
        CHECK_EQUAL(rtrim(str), std::string(" hello"));

        str = " \t \n\t hello";
        CHECK_EQUAL(rtrim(str), std::string(" \t \n\t hello"));

        str = " hello ";
        CHECK_EQUAL(rtrim(str), std::string(" hello"));

        str = " \t \n\t hello \t \n\t ";
        CHECK_EQUAL(rtrim(str), std::string(" \t \n\t hello"));
    }

    TEST(trim_test) {
        std::string str = "hello";
        CHECK_EQUAL(trim(str), std::string("hello"));

        str = "hello ";
        CHECK_EQUAL(trim(str), std::string("hello"));

        str = "hello \t \n\t";
        CHECK_EQUAL(trim(str), std::string("hello"));

        str = " hello";
        CHECK_EQUAL(trim(str), std::string("hello"));

        str = " \t \n\t hello";
        CHECK_EQUAL(trim(str), std::string("hello"));

        str = " hello ";
        CHECK_EQUAL(trim(str), std::string("hello"));

        str = " \t \n\t hello \t \n\t ";
        CHECK_EQUAL(trim(str), std::string("hello"));
    }
}
}  // namespace
