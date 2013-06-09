// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_SYMMETRIC_MS_ONE_TREE_H_
#define TSP_SYMMETRIC_MS_ONE_TREE_H_

#include <cassert>

#include <vector>
#include <limits>
#include <algorithm>

// Minimum Spanning 1-Tree
// based on Prim's algorithm

namespace MSOneTree {
template <typename T>
struct Solution {
    T value;
    std::vector<std::pair<size_t, size_t> > edges;
};

template <typename T>
Solution<T> solve(const T *matrix, size_t size);

}  // namespace MSOneTree
#include "ms_one_tree-inl.h"

#endif  // TSP_SYMMETRIC_MS_ONE_TREE_H_
