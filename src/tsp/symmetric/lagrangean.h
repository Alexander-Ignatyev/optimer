// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_SYMMETRIC_LAGRANGEAN_H_
#define TSP_SYMMETRIC_LAGRANGEAN_H_

#include <vector>

#include <bnb/defs.h>
#include <tsp/symmetric/ms_one_tree.h>

namespace stsp {
    class LagrangeanRelaxation {
    public:
        explicit LagrangeanRelaxation(size_t max_dimension);
        MSOneTree::Solution<value_type> solve(
              const std::vector<value_type> &initial_matrix
              , size_t dimension
              , value_type alpha
              , value_type initial_step
              , size_t max_iter);
    private:
        std::vector<value_type> matrix_;
    };
}  // namespace stsp

#endif  // TSP_SYMMETRIC_LAGRANGEAN_H_
