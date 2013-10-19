// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "common.h"

#include <set>

#include <tsp/common/types.h>

namespace stsp {
std::vector<size_t> build_tour(const tsp::Set &set) {
    std::set<size_t> added_vertices;
    const auto &edges = set.relaxation;
    std::vector<std::vector<size_t> > adjacency_list(edges.size());
    std::vector<size_t> tour;
    tour.reserve(edges.size()+1);
    for (auto &edge : edges) {
        adjacency_list[edge.first].push_back(edge.second);
        adjacency_list[edge.second].push_back(edge.first);
    }
    for (const auto &adj : adjacency_list) {
        if (adj.size() != 2) {
            return tour;
        }
    }
    tour.push_back(0);
    tour.push_back(adjacency_list[0].front());
    added_vertices.insert(adjacency_list[0].front());
    for (size_t i = 1; i < edges.size(); ++i) {
        size_t previous_vertex = tour[tour.size()-2];
        size_t current_vertex = tour.back();
        const auto &adj = adjacency_list[current_vertex];
        if (adj.front() == previous_vertex) {
            tour.push_back(adj.back());
        } else if (adj.back() == previous_vertex) {
            tour.push_back(adj.front());
        } else {
            tour.clear();
            return tour;
        }
        if (added_vertices.find(tour.back()) != added_vertices.end()) {
            tour.clear();
            return tour;
        }
        added_vertices.insert(tour.back());
    }
    return tour;
}
}  // namespace stsp
