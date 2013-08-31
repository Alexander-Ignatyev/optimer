// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "ms_one_tree.h"

namespace MSOneTree {
namespace Prim {

bool vertex_is_included(size_t vertex
                        , const std::vector<size_t> &min_indices) {
    return min_indices[vertex] < min_indices.size();
}

}  // namespace Prim
}  // namespace MSOneTree
