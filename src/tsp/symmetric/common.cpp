// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "common.h"

#include <set>
#include <iostream>

namespace stsp {
    void Solution::write_as_json(std::ostream &os) {
        os << "route = [";
        if (!route.empty()) {
            os << route.front();
            for (size_t i = 1; i < route.size(); ++i) {
                os << ", " << route[i];
            }
        }
        os << "];";
    }

    bool two_opt(const value_type *matrix, size_t dimension, Solution *sol) {
        bool optimized = false;
        bool bContinue = true;

        while (bContinue) {
            bContinue = false;
            auto &route = sol->route;
            for (unsigned first = 1, second = 2;
                 second < route.size() - 2;
                 ++first, ++second) {
                value_type delta = (
                      matrix[route[first - 1]*dimension+route[first]]
                    + matrix[route[first]*dimension+route[second]]
                    + matrix[route[second]*dimension+route[second + 1]])
                    - (matrix[route[first- 1]*dimension+route[second]]
                    + matrix[route[second]*dimension+route[first]]
                    + matrix[route[first]*dimension+route[second + 1]]);
                if (delta > 0) {
                    std::swap(route[first], route[second]);
                    sol->value -= delta;
                    optimized = true;
                    bContinue = true;
                }
            }
        }
        return optimized;
    }

    Solution get_greedy_solution(const value_type *data, size_t dimension
            , size_t start_vertex) {
        Solution sol;
        sol.value = 0;

        std::set<size_t> vertices;
        for (size_t i = 0; i < dimension; ++i) {
                vertices.insert(i);
        }
        size_t curr_vertex = start_vertex;
        size_t min_vertex;
        value_type min_value = M_VAL + 10;

        vertices.erase(curr_vertex);
        sol.route.push_back(curr_vertex);
        while (!vertices.empty()) {
            min_value = M_VAL + 10;
            for (auto vertex : vertices) {
                if (data[curr_vertex*dimension+vertex] < min_value) {
                    min_value = data[curr_vertex*dimension+vertex];
                    min_vertex = vertex;
                }
            }
            sol.value += min_value;
            vertices.erase(min_vertex);
            curr_vertex = min_vertex;
            sol.route.push_back(curr_vertex);
        }
        sol.value += data[curr_vertex*dimension+start_vertex];
        sol.route.push_back(start_vertex);
        return sol;
    }

    Solution get_greedy_solution(const std::vector<value_type> &matrix
        , size_t dimension) {
        Solution best_solution;
        best_solution.value = M_VAL;
        for (unsigned i = 0; i < dimension; ++i) {
            Solution sol = get_greedy_solution(matrix.data(), dimension, i);
            two_opt(matrix.data(), dimension, &sol);
            if (sol.value < best_solution.value) {
                best_solution = sol;
            }
        }
        return best_solution;
    }
}  // namespace stsp
