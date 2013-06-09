//  Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <iostream>
#include <UnitTest++.h>
#include <tsp/symmetric/ms_one_tree.h>

namespace {
    template <typename T>
    void print_solution(std::ostream &os, const T *matrix, size_t size
                , const typename MSOneTree::Solution<T> &solution) {
        os << solution.value << std::endl;
        for (auto edge : solution.edges) {
            os << edge.first << ", " << edge.second << ": ";
            os << matrix[edge.first * size + edge.second] << std::endl;
        }
    }

    TEST(kormen_prim) {
        int matrix[] = {
            // based on Kormen's example
            1000, 1000,   14,    7, 1000, 1000, 1000,    1,    8, 1000,
            1000, 1000,    4, 1000, 1000, 1000, 1000, 1000,    8, 1000,
              14,    4, 1000,    8, 1000, 1000, 1000, 1000,   11, 1000,
               7, 1000,    8, 1000,    7, 1000,    4, 1000, 1000,    2,
            1000, 1000, 1000,    7, 1000,    9,   14, 1000, 1000, 1000,
            1000, 1000, 1000, 1000,    9, 1000,   10, 1000, 1000, 1000,
            1000, 1000, 1000,    4,   14,   10, 1000,    2, 1000, 1000,
               1, 1000, 1000, 1000, 1000, 1000,    2, 1000,    1,    6,
               8,    8,   11, 1000, 1000, 1000, 1000,    1, 1000,    7,
            1000, 1000, 1000,    2, 1000, 1000, 1000,    6,    7, 1000
        };

        size_t size = 10;
        auto solution = MSOneTree::solve(matrix, size);

        CHECK_EQUAL(solution.value, 45);
    }
}  // namespace
