// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <string>
#include <sstream>
#include <iostream>

#include <UnitTest++.h>

#include <common/ini_file.h>

namespace {
SUITE(IniFileTest) {
    TEST(smoke_test) {
        std::stringstream ss_ini;
        ss_ini << "param1=valu\te1" << std::endl;
        ss_ini << "  par\tam2 \t=  value2" << std::endl;
        ss_ini << "   [section]" << std::endl;
        ss_ini << "\t\tparam 3=   value 3" << std::endl;
        ss_ini << "param7=value7" << std::endl;
        ss_ini << "pa\tr4=val4 =value5" << std::endl;
        ss_ini << "[error_section" << std::endl;

        IniFile ini(ss_ini);
        CHECK_EQUAL(ini.num_parse_errors(), 2u);
        CHECK_EQUAL
            (ini.default_section()["param1"], std::string("valu\te1"));
        CHECK_EQUAL
            (ini.default_section()["par\tam2"], std::string("value2"));
        CHECK_EQUAL(ini["section"]["param7"], std::string("value7"));
        CHECK_EQUAL(ini["section"]["param 3"], std::string("value 3"));
    }
}
}  // namespace
