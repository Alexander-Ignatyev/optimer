// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_SYMMETRIC_COMMON_H_
#define TSP_SYMMETRIC_COMMON_H_

#include <vector>

#include <bnb/defs.h>

namespace stsp {
    struct Point {
        Point(const std::pair<size_t, size_t> &pair)
            : x(pair.first)
            , y(pair.second) {}
        size_t x;
        size_t y;
    };

    struct Set {
        value_type value;
        std::vector<std::pair<size_t, size_t> > ms1_solution;
        std::vector<Point> excluded_points;
        unsigned level;
        bool operator < (const Set &other) const {
            return value > other.value;
        }
        void clear() {
            excluded_points.clear();
        }
    };


    struct Solution {
        value_type value;
        std::vector<size_t> route;
    };

    struct InitialData {
        InitialData(const std::vector<value_type> &m, size_t d)
            : matrix(m)
            , rank(d) {}

        const std::vector<value_type> &matrix;
        size_t rank;
    };

    Solution get_greedy_solution(const std::vector<value_type> &matrix
        , size_t dimension);
}  // namespace stsp

#endif  // TSP_SYMMETRIC_COMMON_H_
