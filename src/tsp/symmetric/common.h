// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_SYMMETRIC_COMMON_H_
#define TSP_SYMMETRIC_COMMON_H_

#include <vector>
#include <string>
#include <iosfwd>
#include <unordered_map>

#include <bnb/defs.h>

namespace stsp {

typedef std::pair<size_t, size_t> Edge;

struct Set {
    value_type value;
    std::vector<Edge> relaxation;
    std::vector<Edge> excluded_edges;
    std::vector<Edge> included_edges;
    unsigned level;
    std::vector<size_t> build_tour() const;
    bool operator < (const Set &other) const {
        return value > other.value;
    }
    void clear() {
        excluded_edges.clear();
        included_edges.clear();
    }
};

struct Solution {
    value_type value;
    std::vector<size_t> route;
    void write_as_json(std::ostream &os);
};

struct InitialData {
    InitialData(const std::vector<value_type> &m, size_t d)
        : matrix(m)
        , rank(d) {}

    const std::vector<value_type> &matrix;
    size_t rank;
    std::unordered_map<std::string, std::string> parameters;
};

Solution get_greedy_solution(const std::vector<value_type> &matrix
    , size_t dimension);
}  // namespace stsp

#endif  // TSP_SYMMETRIC_COMMON_H_
