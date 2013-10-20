// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_COMMON_TYPES_H_
#define TSP_COMMON_TYPES_H_

#include <vector>
#include <string>
#include <iosfwd>
#include <unordered_map>

#include <bnb/defs.h>

namespace tsp {
typedef std::pair<size_t, size_t> Edge;

struct Set {
    value_type value;
    std::vector<Edge> relaxation;
    std::vector<Edge> excluded_edges;
    std::vector<Edge> included_edges;
    unsigned level;
    bool operator < (const Set &other) const {
        return value > other.value;
    }
    void clear() {
        relaxation.clear();
        excluded_edges.clear();
        included_edges.clear();
    }
};

struct Solution {
    value_type value;
    std::vector<size_t> route;
    void calc_value(const value_type *matrix, size_t dimension);
    void write_as_json(std::ostream &os);

    bool operator < (const Solution &other) const {
        return value < other.value;
    }
};

struct InitialData {
    InitialData(const std::vector<value_type> &m, size_t d)
                : matrix(m)
                , rank(d) {}
    const std::vector<value_type> &matrix;
    size_t rank;
    std::unordered_map<std::string, std::string> parameters;
};

bool two_opt(const value_type *matrix, size_t dimension, Solution *sol);
Solution get_greedy_solution(const std::vector<value_type> &matrix
                             , size_t dimension);

}  // namespace tsp

#endif  // TSP_COMMON_TYPES_H_
