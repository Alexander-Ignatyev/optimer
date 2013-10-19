// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_SYMMETRIC_COMMON_H_
#define TSP_SYMMETRIC_COMMON_H_

#include <cstddef>
#include <vector>

namespace tsp {
struct Set;
}  // namespace tsp

namespace stsp {
std::vector<size_t> build_tour(const tsp::Set &set);
}  // namespace stsp

#endif  // TSP_SYMMETRIC_COMMON_H_
