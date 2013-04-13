#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <iosfwd>

#include "defs.h"

bool load_tsplib_problem(std::istream &is, value_type *&matrix, size_t &dimension);

#endif //DATA_LOADER_H