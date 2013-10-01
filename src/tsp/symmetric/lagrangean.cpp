// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "lagrangean.h"

#include <cstring>

namespace stsp {
    LagrangeanRelaxation::LagrangeanRelaxation(size_t max_dimension) {
        matrix_.resize(max_dimension * max_dimension);
    }

    MSOneTree::Solution<value_type> LagrangeanRelaxation::solve(
            const std::vector<value_type> &initial_matrix
            , size_t dimension
            , value_type alpha
            , value_type initial_step
            , size_t max_iter) {
        MSOneTree::Solution<value_type> solution;
        std::memcpy(matrix_.data(), initial_matrix.data()
                    , sizeof(value_type)*dimension*dimension);

        std::vector<value_type> lambda(dimension, value_type());
        std::vector<int> gradient(dimension, 2);
        value_type step = initial_step;

        while (max_iter--) {
            solution = MSOneTree::solve(matrix_.data(), dimension);

            std::fill(gradient.begin(), gradient.end(), 2);
            for (const auto &edge : solution.edges) {
                --gradient[edge.first];
                --gradient[edge.second];
            }

            value_type lagragian = 0;
            for (const auto &edge : solution.edges) {
                lagragian += initial_matrix[edge.first*dimension + edge.second];
            }
            for (size_t i = 0; i < gradient.size(); ++i) {
                lagragian -= lambda[i] * gradient[i];
            }

            auto pos = std::find_if(gradient.begin(), gradient.end()
                                    , [](int g) {return g != 0;});
            if (pos == gradient.end()) {
                return solution;
            }

            for (size_t i = 0; i < dimension; ++i) {
                lambda[i] += step*gradient[i];
            }
            step *= alpha;

            for (size_t i = 0; i < dimension; ++i) {
                for (size_t j = 0; j < dimension; ++j) {
                    if (i == j) {
                        matrix_[i*dimension+j] = M_VAL;
                    } else {
                        matrix_[i*dimension+j] = initial_matrix[i*dimension+j]
                        - (lambda[i] + lambda[j]);
                    }
                }
            }
        }
        return solution;
    }
}  // namespace stsp
