// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "stsp.h"

#include <cstring>

#include <g2log.h>
#include <bnb/stats.h>
#include <tsp/symmetric/ms_one_tree.h>

namespace stsp {
ClassicalSolver::ClassicalSolver()
    : dimension_(0)
    , matrix_(nullptr)
    , matrix_original_(nullptr)
    , search_tree_(nullptr) {
}

ClassicalSolver::~ClassicalSolver() {
    delete [] matrix_;
}

void ClassicalSolver::init(const InitialData &data, bnb::SearchTree<Set> *mm) {
    matrix_original_ = data.matrix.data();
    dimension_ = data.rank;
    search_tree_ = mm;
    delete [] matrix_;
    matrix_ = new value_type[dimension_*dimension_];
}
void ClassicalSolver::get_initial_node(Node<Set> *node) {
    node->data.level = 0;
    transform_node(node);
}

void ClassicalSolver::get_initial_solution(Solution *sol) {
    sol->value = M_VAL;
}

void ClassicalSolver::branch(const Node<Set> *node, value_type &record
     , std::vector<Node<Set> *> &nodes, Solution &sol, bnb::Stats &stats) {
    if (node->data.value > record) {
        ++stats.sets_constrained_by_record;
    }
    ++stats.branches;

    auto edges = select_moves(node);
    CHECK(!edges.empty()) << "Cannot select moves";
    for (auto edge : edges) {
        ++stats.sets_generated;
        auto child = search_tree_->create_node(node);
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
    memcpy(matrix_, matrix_original_
           , dimension_*dimension_*sizeof(matrix_original_[0]));
    const Node<Set> *tmp_node = node;
    while (tmp_node->parent) {
        for (const tsp::Edge &edge : tmp_node->data.excluded_edges) {
            matrix_[edge.first*dimension_+edge.second] = M_VAL;
            matrix_[edge.second*dimension_+edge.first] = M_VAL;
        }
        tmp_node = tmp_node->parent;
    }

    auto solution = MSOneTree::solve(matrix_, dimension_);
    node->data.relaxation = std::move(solution.edges);
    node->data.value = solution.value;
}

std::vector<tsp::Edge>
ClassicalSolver::select_moves(const Node<Set> *node) {
    std::vector<tsp::Edge> edges;

    std::vector<size_t> degrees(dimension_, 0);
    for (auto &point : node->data.relaxation) {
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

    for (auto &edge : node->data.relaxation) {
        if (edge.first == selected_vertex
            || edge.second == selected_vertex) {
            edges.push_back(edge);
        }
    }

    return edges;
}

bool ClassicalSolver::build_solution(const Node<Set> *node
                                    , Solution *solution) {
    static const size_t BORDER_VALUE = 2;
    std::vector<size_t> degrees(dimension_, 0);
    for (auto &edge : node->data.relaxation) {
        ++degrees[edge.first];
        ++degrees[edge.second];
    }
    for (size_t i = 0; i < degrees.size(); ++i) {
        size_t degree = degrees[i];
        if (degree != BORDER_VALUE) {
            return false;
        }
    }

    LOG(DEBUG) << "Found solution " << node->data.value;
    solution->value = node->data.value;
    solution->route.clear();
    solution->route.resize(dimension_, dimension_);
    std::vector<size_t> distances(dimension_, dimension_);
    for (auto &edge : node->data.relaxation) {
        if (solution->route[edge.first] == dimension_
                && distances[edge.second] == dimension_) {
            solution->route[edge.first] = edge.second;
            distances[edge.second] = edge.first;
        } else if (solution->route[edge.second] == dimension_
                && distances[edge.first] == dimension_) {
            solution->route[edge.second] = edge.first;
            distances[edge.first] = edge.second;
        } else {
            CHECK(false) << "Incorrect solution's route";
        }
    }
    return true;
}
}  // namespace stsp
