// Copyright (c) 2008-2013 Alexander Ignatyev. All rights reserved.

#ifndef HEURISTICS_ACO_TSP_ANT_H_
#define HEURISTICS_ACO_TSP_ANT_H_

#include <vector>
#include <random>

#include <bnb/defs.h>
#include <tsp/common/types.h>

#include "params.h"

namespace aco_tsp {
class Ant {
 public:
    Ant();
    void init(const std::vector<value_type> &matrix_dist
              , const std::vector<value_type> &matrix_theta
              , size_t dimension
              , const Params &params
              , size_t start_point);

    tsp::Solution solve();

 private:
    size_t get_list_points(size_t *points) const;
    void select_candidate_list(size_t *points
                               , unsigned &npoints, size_t curr_vertex) const;
    size_t get_next_point(size_t curr_vertex);
    void append_vertex(size_t vertex);

    size_t start_point_;
    size_t dimension_;
    size_t candidate_list_size_;
    const value_type *matrix_dist_;
    const value_type *matrix_theta_;
    std::vector<char> selected_vertices_;
    std::vector<size_t> route_;
    std::vector<size_t> candidate_vertices_;
    std::vector<value_type> probabilities_;
    Params params_;
    std::mt19937 random_;
};
}  // namespace aco_tsp
#endif  // HEURISTICS_ACO_TSP_ANT_H_
