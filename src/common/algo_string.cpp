// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#include "algo_string.h"

#include <cstdlib>
#include <stdexcept>

double string_to_double(const std::string &str, double def_val) {
    double result = def_val;
    try {
        result = std::atof(str.c_str());
    } catch(std::exception &) {
        // ignore exception
    }
    return result;
}

size_t string_to_size_t(const std::string &str, size_t def_val) {
    size_t result = def_val;
    try {
        result = std::atol(str.c_str());
    } catch(std::exception &) {
        // ignore exception
    }
    return result;
}
