// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

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
        , size_t max_iter
        , const std::vector<std::pair<size_t, size_t> > &included_edges) {
    static const size_t TOUR_DEGREE = 2;
    static const value_type ALPHA_START_VALUE = 2;

    if (dimension * dimension > matrix_.size()) {
        matrix_.resize(dimension*dimension);
    }

    value_type *matrix = matrix_.data();

    MSOneTree::Solution<value_type> solution;
    std::memcpy(matrix, initial_matrix.data()
                , sizeof(value_type)*dimension*dimension);

    std::vector<int> gradient(dimension);
    std::vector<value_type> lambda(dimension, value_type());

    value_type alpha = ALPHA_START_VALUE;
    value_type alpha_reduce = (alpha-0.01) / max_iter;
    value_type included_edges_penalty = value_type();
    for (size_t iter = 0; iter < max_iter; ++iter) {
        solution = MSOneTree::solve(matrix, dimension);
        solution.value += included_edges_penalty;

        std::fill(gradient.begin(), gradient.end(), 0);
        std::vector<std::pair<size_t, size_t> >::const_iterator pos
            , end = solution.edges.end();
        for (pos = solution.edges.begin(); pos != end; ++pos) {
            ++gradient[pos->first];
            ++gradient[pos->second];
        }

        value_type lagrangean = included_edges_penalty;
        for (pos = solution.edges.begin(); pos != end; ++pos) {
            lagrangean += matrix[pos->first*dimension + pos->second];
        }
        lagrangean -= 2*std::accumulate(lambda.begin()
                                        , lambda.end()
                                        , value_type());

        solution.value = lagrangean;

        if (lagrangean+epsilon > upper_bound) {
            return std::make_pair(solution, iter+1);
        }

        bool isGradientsRightDegree = true;
        for (size_t i = 0; i < gradient.size(); ++i) {
            if (gradient[i] != TOUR_DEGREE) {
                isGradientsRightDegree = false;
                break;
            }
        }
        if (isGradientsRightDegree) {
            solution.value = lagrangean;
            return std::make_pair(solution, iter+1);
        }

        int sum_gradient = 0;
        for (size_t i = 0; i < gradient.size(); ++i) {
            int val = gradient[i] - TOUR_DEGREE;
            sum_gradient += val*val;
        }

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

        included_edges_penalty = value_type();
        for (size_t i = 0; i < included_edges.size(); ++i) {
            const std::pair<size_t, size_t> &edge = included_edges[i];
            size_t index = edge.first*dimension+edge.second;
            included_edges_penalty += matrix[index];
            matrix[index] = value_type();
            matrix[edge.second*dimension+edge.first] = value_type();
        }
    }
    return std::make_pair(solution, max_iter);
}
}  // namespace stsp
