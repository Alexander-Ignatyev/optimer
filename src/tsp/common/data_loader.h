// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_COMMON_DATA_LOADER_H_
#define TSP_COMMON_DATA_LOADER_H_

#include <iosfwd>
#include <vector>

#include <bnb/defs.h>

namespace tsp {
    bool load_tsplib_problem(std::istream &is, std::vector<value_type> &matrix
        , size_t &dimension);
}  // namespace tsp

#endif  // TSP_COMMON_DATA_LOADER_H_
