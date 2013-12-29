// Copyright (c) 2008-2013 Alexander Ignatyev. All rights reserved.

#include "ant.h"

#include <algorithm>

#include <common/log.h>

namespace aco_tsp {
Ant::Ant()
    : start_point_(0)
    , dimension_(0)
    , candidate_list_size_(0)
    , matrix_dist_(nullptr)
    , matrix_theta_(nullptr) {
}

void Ant::init(const std::vector<value_type> &matrix_dist
                , const std::vector<value_type> &matrix_theta
                , size_t dimension
                , const Params &params
                , size_t start_point) {
    start_point_ = start_point;
    dimension_ = dimension;
    candidate_list_size_ =
        static_cast<size_t>(dimension_*params_.candidate_list_factor);
    matrix_dist_ = matrix_dist.data();
    matrix_theta_ = matrix_theta.data();
    selected_vertices_.resize(dimension_);
    route_.reserve(dimension_);
    candidate_vertices_.resize(dimension_);
    probabilities_.resize(dimension_);
    params_ = params;
    random_.seed(start_point_);
}

size_t Ant::get_list_points(size_t *points) const {
    size_t npoints = 0;
    for (size_t i = 0; i < dimension_; ++i) {
        if (selected_vertices_[i] != 1) {
            points[npoints] = i;
            ++npoints;
        }
    }
    return npoints;
}

struct CandidateListFunctor {
    const value_type *dists;
    CandidateListFunctor(const value_type *matrix_dist
                         , size_t dimension, size_t curr_vertex) {
        dists = &matrix_dist[curr_vertex*dimension];
    }

    bool operator()(const size_t &lhs, const size_t &rhs) const {
        return dists[lhs] < dists[rhs];
    }
};


void Ant::select_candidate_list(size_t *points, unsigned &npoints
                                , size_t curr_vertex) const {
    if (candidate_list_size_ < npoints) {
        std::sort(points, points+npoints
                , CandidateListFunctor(matrix_dist_, dimension_, curr_vertex));
        npoints = candidate_list_size_;
    }
}

size_t Ant::get_next_point(size_t curr_vertex) {
    size_t *points = candidate_vertices_.data();
    unsigned npoints = get_list_points(points);
    if (!npoints) {
        return dimension_;
    }
    if (params_.candidate_list_factor > 0) {
        select_candidate_list(points, npoints, curr_vertex);
    }

    value_type *probabilities = probabilities_.data();
    value_type multinv = 0.0f;
    for (unsigned i = 0; i < npoints; ++i) {
        value_type theta = matrix_theta_[curr_vertex * dimension_ + points[i]];
        value_type dist = matrix_dist_[curr_vertex * dimension_ + points[i]];
        probabilities[i] = pow(theta, params_.alpha) / pow(dist, params_.beta);
        multinv += probabilities[i];
    }

    value_type sum = 0;
    size_t result = dimension_;
    unsigned random_value = (random_() - random_.min()) % 100;
    multinv /= 100.0f;
    for (unsigned i = 0; i < npoints; ++i) {
        sum += probabilities[i] / multinv;
        if (sum > random_value) {
            result = points[i];
            break;
        }
    }
    CHECK_F(result < dimension_
            , "Error: Cannot select edge. %f,%f,%d\n", sum, multinv, npoints);
    return result;
}

void Ant::append_vertex(size_t vertex) {
    selected_vertices_[vertex] = 1;
    route_.push_back(vertex);
}

tsp::Solution Ant::solve() {
    std::fill(selected_vertices_.begin(), selected_vertices_.end(), 0);
    route_.clear();
    size_t currVertex = start_point_;
    while (currVertex < dimension_) {
        append_vertex(currVertex);
        currVertex = get_next_point(currVertex);
    }
    append_vertex(route_.front());
    tsp::Solution solution;
    solution.route = route_;
    solution.calc_value(matrix_dist_, dimension_);
    tsp::two_opt(matrix_dist_, dimension_, &solution);
    return solution;
}
}  // namespace aco_tsp
