// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#include "lagrangean_stsp.h"

#include <cstring>

#include <common/log.h>
#include <common/algo_string.h>
#include <bnb/stats.h>

namespace stsp {
LagrangeanSolver::LagrangeanSolver()
    : dimension_(0)
    , search_tree_(0)
    , gradient_max_iters_(100)
    , epsilon_(0) {
}

void LagrangeanSolver::init(const InitialData &data, bnb::SearchTree<Set> *mm) {
    matrix_original_ = data.matrix;
    dimension_ = data.rank;
    search_tree_ = mm;
    matrix_.resize(dimension_*dimension_);
    edge_map_.resize(matrix_.size());
    solution_initial_ = tsp::get_greedy_solution(matrix_original_, dimension_);
    LOG(INFO) << "Initial solution: " << solution_initial_.value;
    std::ostringstream oss;
    solution_initial_.write_as_json(oss);
    LOG(INFO) << oss.str();

    std::map<std::string, std::string>::const_iterator pos;
    pos = data.parameters.find("epsilon");
    if (pos != data.parameters.end()) {
        double epsilon = string_to_double(pos->second, epsilon_);
        epsilon_ = static_cast<value_type>(epsilon);
    }

    pos = data.parameters.find("gradinet_max_iters");
    if (pos != data.parameters.end()) {
        gradient_max_iters_ = string_to_size_t(pos->second
                                , gradient_max_iters_);
    }

    branching_rule = &LagrangeanSolver::branching_rule1;
    pos = data.parameters.find("branching_rule");
    if (pos != data.parameters.end()) {
        size_t rule = string_to_size_t(pos->second, 1);
        if (rule == 2) {
            branching_rule = &LagrangeanSolver::branching_rule2;
        } else if (rule == 3) {
            branching_rule = &LagrangeanSolver::branching_rule3;
        }
    }
}

void LagrangeanSolver::get_initial_node(Node *node) {
    node->data.level = 0;
    bnb::Stats stats;
    transform_node(node, solution_initial_.value, stats);
}

void LagrangeanSolver::get_initial_solution(Solution *sol) {
    *sol = solution_initial_;
}

void LagrangeanSolver::branch(const Node *node, value_type &record
        , NodeList &nodes, Solution &sol, bnb::Stats &stats) {
    if (node->data.value+epsilon_ >= record) {
        ++stats.sets_constrained_by_record;
        return;
    }
    ++stats.branches;

    NodeList new_nodes = (this->*branching_rule)(node);

    // transform nodes
    for (size_t i = 0; i < new_nodes.size(); ++i) {
        Node *child = new_nodes[i];
        ++stats.sets_generated;
        transform_node(child, record, stats);
        if (child->data.value+epsilon_ >= record) {
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

LagrangeanSolver::NodeList
LagrangeanSolver::branching_rule1(const Node *node) {
    std::vector<tsp::Edge> moves;

    std::vector<size_t> degrees(dimension_, 0);
    for (size_t i = 0; i < node->data.relaxation.size(); ++i) {
        const tsp::Edge &edge = node->data.relaxation[i];
        ++degrees[edge.first];
        ++degrees[edge.second];
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
            moves.push_back(edge);
        }
    }

    NodeList created_nodes;

    for (size_t i = 0; i < moves.size(); ++i) {
        const tsp::Edge &move = moves[i];
        Node *child = search_tree_->create_node(node);
        child->data.level = node->data.level+1;
        child->data.excluded_edges.push_back(move);
        created_nodes.push_back(child);
    }

    return created_nodes;
}

// Volgenant and Jonker, 1982
LagrangeanSolver::NodeList LagrangeanSolver::branching_rule2(const Node *node) {
    get_included_edges(node, edge_map_);

    std::vector<size_t> total_degrees(dimension_, 0);
    std::vector<size_t> free_degrees(dimension_, 0);
    for (size_t i = 0; i < node->data.relaxation.size(); ++i) {
        const tsp::Edge &edge = node->data.relaxation[i];
        ++total_degrees[edge.first];
        ++total_degrees[edge.second];
        // if edge is not included
        if (edge_map_[edge.first*dimension_+edge.second] == 0) {
            ++free_degrees[edge.first];
            ++free_degrees[edge.second];
        }
    }

    static const size_t BORDER_VALUE = 2;
    size_t selected_vertex = 0;
    for (size_t i = 0; i < total_degrees.size(); ++i) {
        if (total_degrees[i] > BORDER_VALUE
            && free_degrees[i] >= BORDER_VALUE) {
            selected_vertex = i;
            break;
        }
    }

    std::vector<tsp::Edge> selected_edges;
    for (size_t i = 0; i < node->data.relaxation.size(); ++i) {
        const tsp::Edge &edge = node->data.relaxation[i];
        bool is_adj_edge = (edge.first == selected_vertex)
                        || (edge.second == selected_vertex);
        if (is_adj_edge
                && (edge_map_[edge.first*dimension_+edge.second] == 0)) {
            selected_edges.push_back(edge);
            if (selected_edges.size() == BORDER_VALUE) {
                break;
            }
        }
    }

    CHECK(selected_edges.size() == BORDER_VALUE);

    NodeList created_nodes;
    if (total_degrees[selected_vertex] == free_degrees[selected_vertex]) {
        // node #1
        Node *child = search_tree_->create_node(node);
        child->data.level = node->data.level+1;
        child->data.included_edges.push_back(selected_edges.front());
        child->data.included_edges.push_back(selected_edges.back());
        created_nodes.push_back(child);
    }

    // node #2
    Node *child = search_tree_->create_node(node);
    child->data.level = node->data.level+1;
    child->data.included_edges.push_back(selected_edges.front());
    child->data.excluded_edges.push_back(selected_edges.back());
    created_nodes.push_back(child);

    // node #3
    child = search_tree_->create_node(node);
    child->data.level = node->data.level+1;
    child->data.excluded_edges.push_back(selected_edges.front());
    created_nodes.push_back(child);

    return created_nodes;
}

// select the edge with maximum weight and endpoint degree > 2
LagrangeanSolver::NodeList LagrangeanSolver::branching_rule3(const Node *node) {
    static const size_t BORDER_VALUE = 2;
    get_included_edges(node, edge_map_);
    std::vector<size_t> degrees(dimension_);
    for (size_t i = 0; i < node->data.relaxation.size(); ++i) {
        const tsp::Edge &edge = node->data.relaxation[i];
        ++degrees[edge.first];
        ++degrees[edge.second];
    }

    tsp::Edge selected_edge(0, 0);
    value_type max_length = -1e16;
    for (size_t i = 0; i < node->data.relaxation.size(); ++i) {
        const tsp::Edge &edge = node->data.relaxation[i];
        // if edge is included
        if (edge_map_[edge.first*dimension_ + edge.second] != 0) {
            continue;
        }
        value_type length = matrix_original_[edge.first*dimension_+edge.second];
        if (degrees[edge.first] > BORDER_VALUE) {
            if (length > max_length) {
                max_length = length;
                selected_edge = edge;
            }
        } else if (degrees[edge.second] > BORDER_VALUE) {
            if (length > max_length) {
                max_length = length;
                selected_edge = edge;
            }
        }
    }
    NodeList created_nodes;
    if (selected_edge.first == 0 && selected_edge.second == 0) {
        return created_nodes;
    }
    Node *child = search_tree_->create_node(node);
    child->data.level = node->data.level+1;
    child->data.excluded_edges.push_back(selected_edge);
    created_nodes.push_back(child);

    child = search_tree_->create_node(node);
    child->data.level = node->data.level+1;
    child->data.included_edges.push_back(selected_edge);
    created_nodes.push_back(child);
    return created_nodes;
}

void LagrangeanSolver::transform_node(Node *node
            , value_type record, bnb::Stats &stats) {
    // restore excluded edgess from branch
    memcpy(matrix_.data(), matrix_original_.data()
           , matrix_original_.size()*sizeof(matrix_original_[0]));
    std::vector<tsp::Edge> included_edges;
    const Node *tmp_node = node;
    std::vector<std::vector<size_t> > adjacency_list(dimension_);
    while (tmp_node->parent) {
        for (size_t i = 0; i < tmp_node->data.excluded_edges.size(); ++i) {
            const tsp::Edge &edge = tmp_node->data.excluded_edges[i];
            matrix_[edge.first*dimension_+edge.second] = M_VAL;
            matrix_[edge.second*dimension_+edge.first] = M_VAL;
        }
        for (size_t i = 0; i < tmp_node->data.included_edges.size(); ++i) {
            const tsp::Edge &edge = tmp_node->data.included_edges[i];
            included_edges.push_back(std::make_pair(edge.first, edge.second));
            adjacency_list[edge.first].push_back(edge.second);
            adjacency_list[edge.second].push_back(edge.first);
        }
        tmp_node = tmp_node->parent;
    }

    for (size_t u = 0; u < dimension_; ++u) {
        CHECK(adjacency_list[u].size() < 3);
        if (adjacency_list[u].size() == 2) {
            for (size_t v = 0; v < dimension_; ++v) {
                if (v != adjacency_list[u].front()
                    && v != adjacency_list[u].back()) {
                    matrix_[u*dimension_+v] = M_VAL;
                    matrix_[v*dimension_+u] = M_VAL;
                }
            }
        }
    }

    std::pair<MSOneTree::Solution<value_type>, size_t> solution =
    lr_.solve(matrix_, dimension_, record
                , epsilon_, gradient_max_iters_, included_edges);

    node->data.relaxation = solution.first.edges;
    node->data.value = solution.first.value;
    stats.bound_problems_solved += solution.second;
}

bool LagrangeanSolver::build_solution(const Node *node, Solution *solution) {
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

void LagrangeanSolver::get_included_edges(
                                const bnb::Node<LagrangeanSolver::Set> *node
                                , std::vector<int> &edge_map) const {
    memset(&edge_map[0], 0, sizeof(edge_map[0])*edge_map.size());
    const bnb::Node<LagrangeanSolver::Set> *tmp_node = node;
    while (tmp_node->parent) {
        for (size_t i = 0; i < tmp_node->data.included_edges.size(); ++i) {
            const tsp::Edge &edge = tmp_node->data.included_edges[i];
            edge_map[edge.first*dimension_ + edge.second] = 1;
            edge_map[edge.second*dimension_ + edge.first] = 1;
        }
        tmp_node = tmp_node->parent;
    }
}
}  // namespace stsp
