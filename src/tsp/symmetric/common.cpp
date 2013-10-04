// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "common.h"

#include <set>

namespace stsp {
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
            if (sol.value < best_solution.value) {
                best_solution = sol;
            }
        }
        return best_solution;
    }
}  // namespace stsp
