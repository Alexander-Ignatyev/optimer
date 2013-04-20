// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_DATA_LOADER_H_
#define SRC_DATA_LOADER_H_

#include <iosfwd>

#include "defs.h"

bool load_tsplib_problem(std::istream &is, value_type *&matrix
    , size_t &dimension);

#endif  // SRC_DATA_LOADER_H_
