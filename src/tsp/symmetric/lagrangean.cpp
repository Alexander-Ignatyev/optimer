// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "lagrangean.h"

#include <cstring>
#include <cmath>

#include <numeric>

namespace stsp {
std::pair<MSOneTree::Solution<value_type>, size_t> LagrangeanRelaxation::solve(
        const std::vector<value_type> &initial_matrix
        , size_t dimension
        , value_type upper_bound
        , value_type epsilon
        , size_t max_iter) {
    static const size_t TOUR_DEGREE = 2;
    static const value_type ALPHA_START_VALUE = 2;

    if (dimension * dimension > matrix_.size()) {
        matrix_.resize(dimension*dimension);
    }

    value_type *matrix = matrix_.data();

    MSOneTree::Solution<value_type> solution;
    std::memcpy(matrix, initial_matrix.data()
                , sizeof(value_type)*dimension*dimension);

    std::vector<value_type> lambda(dimension, value_type());
    std::vector<int> gradient(dimension);

    value_type alpha = ALPHA_START_VALUE;
    value_type alpha_reduce = (alpha-0.01) / max_iter;
    for (size_t iter = 0; iter < max_iter; ++iter) {
        solution = MSOneTree::solve(matrix, dimension);

        std::fill(gradient.begin(), gradient.end(), 0);
        for (const auto &edge : solution.edges) {
            ++gradient[edge.first];
            ++gradient[edge.second];
        }

        value_type lagrangean = 0;
        for (const auto &edge : solution.edges) {
            lagrangean += matrix[edge.first*dimension + edge.second];
        }
        lagrangean -= 2*std::accumulate(lambda.begin()
                                        , lambda.end()
                                        , value_type());

        solution.value = lagrangean;

        if (lagrangean+epsilon > upper_bound) {
            return std::make_pair(solution, iter+1);
        }

        auto pos = std::find_if(gradient.begin(), gradient.end()
                                , [](int g) {return g != TOUR_DEGREE;});
        if (pos == gradient.end()) {
            solution.value = lagrangean;
            return std::make_pair(solution, iter+1);
        }

        value_type sum_gradient = std::accumulate(gradient.begin()
                , gradient.end()
                , value_type()
                , [](const value_type &sum, const value_type &val) {
                    return sum + (val-TOUR_DEGREE)*(val-TOUR_DEGREE);
                });
        value_type step = alpha*(upper_bound - lagrangean)/sum_gradient;

        for (size_t i = 0; i < dimension; ++i) {
            lambda[i] += step*gradient[i];
        }

        alpha -= alpha_reduce;

        for (size_t i = 0; i < dimension; ++i) {
            for (size_t j = 0; j < dimension; ++j) {
                if (i == j) {
                    matrix[i*dimension+j] = M_VAL;
                } else {
                    matrix[i*dimension+j] = initial_matrix[i*dimension+j]
                    + (lambda[i] + lambda[j]);
                }
            }
        }
    }
    return std::make_pair(solution, max_iter);
}
}  // namespace stsp
