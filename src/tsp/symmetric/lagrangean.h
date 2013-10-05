// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_SYMMETRIC_LAGRANGEAN_H_
#define TSP_SYMMETRIC_LAGRANGEAN_H_

#include <vector>

#include <bnb/defs.h>
#include <tsp/symmetric/ms_one_tree.h>

namespace stsp {
    class LagrangeanRelaxation {
    public:
        std::pair<MSOneTree::Solution<value_type>, size_t> solve(
              const std::vector<value_type> &initial_matrix
              , size_t dimension
              , value_type upper_bound
              , size_t max_iter);
    private:
        std::vector<value_type> matrix_;
    };
}  // namespace stsp

#endif  // TSP_SYMMETRIC_LAGRANGEAN_H_
