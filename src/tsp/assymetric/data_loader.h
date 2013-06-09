// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_ASSYMETRIC_DATA_LOADER_H_
#define TSP_ASSYMETRIC_DATA_LOADER_H_

#include <iosfwd>

#include <bnb/defs.h>

bool load_tsplib_problem(std::istream &is, value_type *&matrix
    , size_t &dimension);

#endif  // TSP_ASSYMETRIC_DATA_LOADER_H_
