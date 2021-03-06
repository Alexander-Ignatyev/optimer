// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#include "stsp.h"

#include <cstring>
#include <sstream>

#include <common/log.h>
#include <bnb/stats.h>
#include <tsp/symmetric/ms_one_tree.h>
#include <tsp/symmetric/common.h>

namespace stsp {
ClassicalSolver::ClassicalSolver()
    : dimension_(0)
    , search_tree_(0) {
}

void ClassicalSolver::init(const InitialData &data, bnb::SearchTree<Set> *mm) {
    matrix_original_ = data.matrix;
    dimension_ = data.rank;
    search_tree_ = mm;
    matrix_.resize(dimension_*dimension_);
}
void ClassicalSolver::get_initial_node(Node<Set> *node) {
    node->data.level = 0;
    transform_node(node);
}

void ClassicalSolver::get_initial_solution(Solution *sol) {
    *sol = tsp::get_greedy_solution(matrix_original_, dimension_);
}

void ClassicalSolver::branch(const Node<Set> *node, value_type &record
     , std::vector<Node<Set> *> &nodes, Solution &sol, bnb::Stats &stats) {
    if (node->data.value > record) {
        ++stats.sets_constrained_by_record;
    }
    ++stats.branches;

    std::vector<tsp::Edge> edges = select_moves(node);
    CHECK(!edges.empty()) << "Cannot select moves";
    for (size_t i = 0; i < edges.size(); ++i) {
        const tsp::Edge &edge = edges[i];
        ++stats.sets_generated;
        Node<Set> *child = search_tree_->create_node(node);
        child->data.level = node->data.level+1;
        child->data.excluded_edges.push_back(edge);
        transform_node(child);
        if (child->data.value >= record) {
            ++stats.sets_constrained_by_record;
            search_tree_->release_node(child);
        } else {
            if (build_solution(child, &sol)) {
                record = sol.value;
                search_tree_->release_node(child);
            } else {
                nodes.push_back(child);
            }
        }
    }
}

void ClassicalSolver::transform_node(Node<Set> *node) {
    // restore excluded points from branch
    memcpy(matrix_.data(), matrix_original_.data()
           , dimension_*dimension_*sizeof(matrix_original_[0]));
    const Node<Set> *tmp_node = node;
    while (tmp_node->parent) {
        for (size_t i = 0; i < tmp_node->data.excluded_edges.size(); ++i) {
            const tsp::Edge &edge = tmp_node->data.excluded_edges[i];
            matrix_[edge.first*dimension_+edge.second] = M_VAL;
            matrix_[edge.second*dimension_+edge.first] = M_VAL;
        }
        tmp_node = tmp_node->parent;
    }

    MSOneTree::Solution<value_type> solution
        = MSOneTree::solve(matrix_.data(), dimension_);
    node->data.relaxation = solution.edges;
    node->data.value = solution.value;
}

std::vector<tsp::Edge>
ClassicalSolver::select_moves(const Node<Set> *node) {
    std::vector<tsp::Edge> edges;

    std::vector<size_t> degrees(dimension_, 0);
    for (size_t i = 0; i < node->data.relaxation.size(); ++i) {
        const tsp::Edge &point = node->data.relaxation[i];
        ++degrees[point.first];
        ++degrees[point.second];
    }

    static const size_t BORDER_VALUE = 2;
    size_t selected_vertex = 0;
    size_t selected_degree = degrees.size();
    for (size_t i = 0; i < degrees.size(); ++i) {
        size_t degree = degrees[i];
        if (degree > BORDER_VALUE && selected_degree > degree) {
            selected_degree = degree;
            selected_vertex = i;
        }
    }

    for (size_t i = 0; i < node->data.relaxation.size(); ++i) {
        const tsp::Edge &edge = node->data.relaxation[i];
        if (edge.first == selected_vertex
            || edge.second == selected_vertex) {
            edges.push_back(edge);
        }
    }

    return edges;
}

bool ClassicalSolver::build_solution(const Node<Set> *node
                                    , Solution *solution) {
    solution->route = build_tour(node->data);
    if (solution->route.empty()) {
        return false;
    }

    LOG(INFO) << "Found solution " << node->data.value;
    solution->value = node->data.value;
    std::ostringstream oss;
    solution->write_as_json(oss);
    LOG(INFO) << oss.str();
    return true;
}
}  // namespace stsp
