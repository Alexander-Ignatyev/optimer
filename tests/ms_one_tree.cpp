//  Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#include <iostream>
#include <limits>
#include <UnitTest++.h>
#include <tsp/symmetric/ms_one_tree.h>

namespace {
template <typename T>
void print_solution(std::ostream &os, const T *matrix, size_t size
            , const typename MSOneTree::Solution<T> &solution) {
    os << solution.value << std::endl;
    for (size_t i = 0; i < solution.edges.size(); ++i) {
        const std::pair<size_t, size_t> &edge = solution.edges[i];
        os << (edge.first+1) << ",\t" << (edge.second+1) << ":\t";
        os << matrix[edge.first * size + edge.second] << "\t\t\t";
        os << edge.first << ",\t" << edge.second;
        os << std::endl;
    }
}

template <typename T>
T max_val() {
    static T m = std::numeric_limits<T>::max() / 2;
    return m;
}

template <typename T>
void exclude_edges(T *matrix, size_t size
                   , std::vector<std::pair<size_t, size_t> > &excluded_edges) {
    for (size_t i = 0; i < excluded_edges.size(); ++i) {
        const std::pair<size_t, size_t> &edge = excluded_edges[i];
        matrix[edge.first*size + edge.second] = max_val<T>();
        matrix[edge.second*size + edge.first] = max_val<T>();
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
    MSOneTree::Solution<int> solution = MSOneTree::solve(matrix, size);

    CHECK_EQUAL(45, solution.value);
}

TEST(nasini12_prim) {
    using std::make_pair;
    static const int M_VAL = max_val<int>();
    int matrix[] =  {
        M_VAL, 750, 431, 402, 133, 489, 310, 566, 302, 214, 785, 762,
        750, M_VAL, 736, 671, 85, 174, 870, 927, 683, 336, 882, 150,
        431, 736, M_VAL, 117, 934, 65, 939, 305, 422, 291, 88, 507,
        402, 671, 117, M_VAL, 982, 727, 911, 870, 380, 754, 367, 580,
        133, 85, 934, 982, M_VAL, 956, 834, 118, 795, 633, 447, 446,
        489, 174, 65, 727, 956, M_VAL, 322, 397, 356, 336, 27, 872,
        310, 870, 939, 911, 834, 322, M_VAL, 127, 262, 821, 776, 81,
        566, 927, 305, 870, 118, 397, 127, M_VAL, 257, 429, 320, 524,
        302, 683, 422, 380, 795, 356, 262, 257, M_VAL, 555, 244, 290,
        214, 336, 291, 754, 633, 336, 821, 429, 555, M_VAL, 508, 77,
        785, 882, 88, 367, 447, 27, 776, 320, 244, 508, M_VAL, 293,
        762, 150, 507, 580, 446, 872, 81, 524, 290, 77, 293, M_VAL
    };
    size_t size = 12;

    std::vector<std::pair<size_t, size_t> > excluded_edges;
    MSOneTree::Solution<int> solution = MSOneTree::solve(matrix, size);
    CHECK_EQUAL(1462, solution.value);

    excluded_edges.push_back(make_pair(0, 4));
    exclude_edges(matrix, size, excluded_edges);
    solution = MSOneTree::solve(matrix, size);
    CHECK_EQUAL(1631, solution.value);

    excluded_edges.push_back(make_pair(11, 6));
    exclude_edges(matrix, size, excluded_edges);
    solution = MSOneTree::solve(matrix, size);
    CHECK_EQUAL(1700, solution.value);

    excluded_edges.push_back(make_pair(1, 4));
    exclude_edges(matrix, size, excluded_edges);
    solution = MSOneTree::solve(matrix, size);
    CHECK_EQUAL(1872, solution.value);

    excluded_edges.push_back(make_pair(2, 5));
    exclude_edges(matrix, size, excluded_edges);
    solution = MSOneTree::solve(matrix, size);
    CHECK_EQUAL(1895, solution.value);

    excluded_edges.push_back(make_pair(7, 8));
    exclude_edges(matrix, size, excluded_edges);
    solution = MSOneTree::solve(matrix, size);
    CHECK_EQUAL(1900, solution.value);
}
}  // namespace
