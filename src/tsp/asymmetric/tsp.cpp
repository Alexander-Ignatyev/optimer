// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "tsp.h"

#include <cstring>

#include <algorithm>
#include <vector>
#include <set>
#include <unordered_set>

#include <g2log.h>

#include <bnb/stats.h>

namespace {
bool is_m(value_type val) {
    return val >= M_VAL;
}

}

struct EdgeHash {
    std::hash<decltype(tsp::Edge::first)> hash_x;
    std::hash<decltype(tsp::Edge::second)> hash_y;
    size_t operator()(const tsp::Edge &edge) const {
        return hash_x(edge.first+edge.second) ^ hash_y(edge.second);
    }
};

struct EdgeEqualsTo {
    bool operator()(const tsp::Edge &lhs
        , const tsp::Edge &rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

TspSolver::TspSolver()
    : dimension_(0)
    , matrix_(nullptr)
    , matrix_original_(nullptr)
    , search_tree_(nullptr) { }

void TspSolver::init(const tsp::InitialData &data, bnb::SearchTree<Set> *mm) {
    matrix_original_ = data.matrix.data();
    dimension_ = data.rank;
    search_tree_ = mm;
    matrix_ = new value_type[dimension_*dimension_];
    initial_solution_ = tsp::get_greedy_solution(data.matrix, data.rank);
}

void TspSolver::get_initial_node(Node *node) {
    node->data.level = 0;
    transform_node(node);
}

void TspSolver::get_initial_solution(Solution *sol) {
    *sol = initial_solution_;
}

bool has_solution(const std::vector<size_t> &ap_sol, size_t dimension) {
    size_t first = 0;
    size_t last = ap_sol[first];
    size_t n = 1;
    while (first != last && n < dimension) {
        last = ap_sol[last];
        ++n;
    }
    return first == last && n == dimension;
}

void TspSolver::branch(const Node *node, value_type &record,
    NodeList &nodes, Solution &sol, bnb::Stats &stats) {

    if (node->data.value >= record) {
        ++stats.sets_constrained_by_record;
        return;
    }
    stats.branches++;
    std::vector<tsp::Edge> edges;
    if (!select_move(*node, &edges)) {
        LOG(WARNING) << "TSP AP: Error! Cannot select edge";
        dump_to_log(node);
        CHECK(false);
        return;
    }

    std::vector<tsp::Edge> included_edges;
    for (auto edge : edges) {
        Node *new_node = search_tree_->create_node(node);
        new_node->data.level = node->data.level+1;
        new_node->data.included_edges = included_edges;
        new_node->data.excluded_edges.push_back(edge);
        transform_node(new_node);
        included_edges.push_back(edge);
        ++stats.sets_generated;

        if (has_solution(new_node->data.ap_solution.primal, dimension_)) {
            if (new_node->data.value < record) {
                record = new_node->data.value;
                LOG(INFO) << "ATSP AP: found new record: "
                    << record << ". Level: " << new_node->data.level;
                sol.route = create_tour(new_node->data.ap_solution.primal);
                sol.value = record;
                check_route(sol.route, new_node);
            }
            search_tree_->release_node(new_node);
            continue;
        }

        if (new_node->data.value < record) {
            nodes.push_back(new_node);
        } else {
            if (is_m(new_node->data.value)) {
                LOG(INFO) << "branch: sets_constrained_by_record with M_VAL";
            }
            ++stats.sets_constrained_by_record;
            search_tree_->release_node(new_node);
        }
    }
}

std::vector<size_t> TspSolver::create_tour(const std::vector<size_t> &ap_sol) {
    std::vector<size_t> tour;
    size_t start = 0;
    tour.push_back(start);
    size_t finish = ap_sol[start];
    tour.push_back(finish);
    size_t counter = 1;
    size_t length = ap_sol.size();
    while (start != finish && counter < length) {
        ++counter;
        finish = ap_sol[finish];
        tour.push_back(finish);
    }
    if (counter != length) {
        tour.clear();
    }
    return tour;
}

bool TspSolver::select_move(const Node &node
                            , std::vector<tsp::Edge> *edges) const {
    std::vector<char> selected_vertices(dimension_, 0);
    std::unordered_set<tsp::Edge, EdgeHash, EdgeEqualsTo> included_edges;
    const Node *tmp_node = &node;
    while (tmp_node->parent) {
        included_edges.insert(
            tmp_node->data.included_edges.begin()
            , tmp_node->data.included_edges.end());
        tmp_node = tmp_node->parent;
    }
    size_t selected_path_start = dimension_;
    size_t selected_path_length = M_VAL;
    auto ap_solution = node.data.ap_solution.primal.data();
    while (selected_path_length != 2) {
        size_t start = 0;
        for (; start != dimension_ && selected_vertices[start] != 0; ++start) {}
        if (start == dimension_) {
            break;
        }
        size_t path_length = 1;
        ++selected_vertices[start];
        size_t finish = ap_solution[start];
        ++selected_vertices[finish];
        for (; start != finish && path_length < dimension_; ++path_length) {
            finish = ap_solution[finish];
            ++selected_vertices[finish];
        }
        if (start == finish && path_length > 1
                && path_length < selected_path_length) {
            selected_path_start = start;
            selected_path_length = path_length;
        }
    }

    if (selected_path_start < dimension_) {
        auto start = selected_path_start;
        auto finish = ap_solution[start];
        for (; selected_path_start != finish; ) {
            tsp::Edge edge = {start, finish};
            if (included_edges.find(edge) == included_edges.end()) {
                edges->push_back(edge);
            }
            start = finish;
            finish = ap_solution[finish];
        }
        tsp::Edge edge = {start, finish};
        edges->push_back(edge);
    }
    return !edges->empty();
}

void TspSolver::transform_node(Node *node) {
    // restore included and excluded points from branch
    std::vector<tsp::Edge> included_points;
    std::vector<tsp::Edge> excluded_points;
    const Node *tmp_node = node;
    while (tmp_node->parent) {
        included_points.insert(included_points.end()
            , tmp_node->data.included_edges.begin()
            , tmp_node->data.included_edges.end());
        excluded_points.insert(excluded_points.end()
            , tmp_node->data.excluded_edges.begin()
            , tmp_node->data.excluded_edges.end());
        tmp_node = tmp_node->parent;
    }

    std::vector<size_t> i_original_to_new(dimension_, 0);
    std::vector<size_t> j_original_to_new(dimension_, 0);
    for (const tsp::Edge &edge : included_points) {
        i_original_to_new[edge.first] = dimension_;
        j_original_to_new[edge.second] = dimension_;
    }

    size_t i_new = 0;
    size_t j_new = 0;
    for (size_t k_original = 0; k_original < dimension_; ++k_original) {
        if (i_original_to_new[k_original] < dimension_) {
            i_original_to_new[k_original] = i_new;
            ++i_new;
        }
        if (j_original_to_new[k_original] < dimension_) {
            j_original_to_new[k_original] = j_new;
            ++j_new;
        }
    }

    size_t dimension_new = dimension_ - included_points.size();
    std::vector<size_t> j_new_to_original(dimension_new);
    j_new = 0;
    for (size_t j_original = 0; j_original < dimension_; ++j_original) {
        if (j_original_to_new[j_original] < dimension_) {
            j_new_to_original[j_new] = j_original;
            ++j_new;
        }
    }

    // prepare matrix for AP_Solver
    i_new = 0;
    for (size_t i_original = 0; i_original < dimension_; ++i_original) {
        if (i_original_to_new[i_original] == dimension_) {
            continue;
        }
        j_new = 0;
        for (size_t j_original = 0; j_original < dimension_; ++j_original) {
            if (j_original_to_new[j_original] == dimension_) {
                continue;
            }
            matrix_[i_new * dimension_new + j_new]
                = matrix_original_[i_original * dimension_ + j_original];
            ++j_new;
        }
        ++i_new;
    }

    for (const tsp::Edge &edge : excluded_points) {
        if (i_original_to_new[edge.first] == dimension_
            || j_original_to_new[edge.second] == dimension_) {
                continue;
            }
        size_t index = i_original_to_new[edge.first] * dimension_new;
        index += j_original_to_new[edge.second];
        matrix_[index] = M_VAL;
    }

    // preprocess of AP
    ApSolver<value_type>::Solution ap_solution_new;
    if (node->parent) {
        ap_solution_new.init(dimension_new);
        const auto &tmp = node->parent->data.ap_solution;
        i_new = 0;
        for (size_t i_original = 0; i_original < dimension_; ++i_original) {
            if (i_original_to_new[i_original] != dimension_) {
                size_t j_original = tmp.primal[i_original];
                ap_solution_new.primal[i_new] = j_original_to_new[j_original];
                ap_solution_new.dual_u[i_new] = tmp.dual_u[i_original];
                ++i_new;
            }
        }
        j_new = 0;
        for (size_t j_original = 0; j_original < dimension_; ++j_original) {
            if (j_original_to_new[j_original] < dimension_) {
                ap_solution_new.dual_v[j_new] = tmp.dual_v[j_original];
                ++j_new;
            }
        }
        for (auto edge : node->data.excluded_edges) {
            i_new = i_original_to_new[edge.first];
            j_new = j_original_to_new[edge.second];
            CHECK(i_new < dimension_new);
            CHECK(j_new < dimension_new);
            ap_solution_new.primal[i_new] = dimension_new;
        }
    }

    ap_solver_.solve(matrix_, dimension_new, &ap_solution_new);

    const auto ap_primal_new = ap_solution_new.primal.data();
    const auto ap_dual_u_new = ap_solution_new.dual_u.data();
    const auto ap_dual_v_new = ap_solution_new.dual_v.data();

    node->data.ap_solution.init(dimension_);

    auto ap_primal_original = node->data.ap_solution.primal.data();
    auto ap_dual_u_original = node->data.ap_solution.dual_u.data();
    auto ap_dual_v_original = node->data.ap_solution.dual_v.data();

    i_new = 0;
    for (size_t i_original = 0; i_original < dimension_; ++i_original) {
        if (i_original_to_new[i_original] == dimension_) {
            continue;
        }
        size_t j_new = ap_primal_new[i_new];
        size_t j_original = j_new_to_original[j_new];
        ap_primal_original[i_original] = j_original;
        ap_dual_u_original[i_original] = ap_dual_u_new[i_new];
        ++i_new;
    }
    j_new = 0;
    for (size_t j_original = 0; j_original < dimension_; ++j_original) {
        if (j_original_to_new[j_original] == dimension_) {
            continue;
        }
        ap_dual_v_original[j_original] = ap_dual_v_new[j_new];
        ++j_new;
    }
    for (auto edge : included_points) {
        ap_primal_original[edge.first] = edge.second;
    }

    // calculate value
    value_type value = 0;
    for (size_t i = 0; i < dimension_; ++i) {
        auto cost = matrix_original_[i*dimension_ + ap_primal_original[i]];
        LOG_IF(WARNING, is_m(cost)) << "transform_node: invalid ap_solve (" <<
            i << ", " << ap_primal_original[i] << ")";
        value += cost;
    }
    node->data.value = value;
}

void TspSolver::print_matrix(const value_type *matrix
    , size_t dimension, std::ostream &os) {
    for (size_t i = 0; i < dimension; ++i) {
        for (size_t j = 0; j < dimension; ++j) {
            if (!is_m(matrix[i*dimension+j])) {
                os << matrix[i*dimension+j] << " ";
            } else {
                os << "M ";
            }
        }
        os << std::endl;
    }
    os << std::endl;
}

void TspSolver::check_route(const decltype(Solution::route) &route
    , const Node *node) {
    if (route.empty()) {
        dump_to_log(node);
        LOG(FATAL) << "Cannot build a tour. Error AP solve\n";
    }
}

void TspSolver::dump_to_log(const Node *node) {
    std::ostringstream oss;
    oss << node->data;
    print_matrix(matrix_original_, dimension_, oss);
    LOG(WARNING) << oss.str();
    oss.str(std::string());
    if (node->parent != nullptr) {
        dump_to_log(node->parent);
    }
}

std::ostream &operator<<(std::ostream &os, const tsp::Edge &edge) {
    os << "(" << edge.first << ", " << edge.second << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const TspSolver::Set &set) {
    os << "<< Set: " << std::endl;
    os << "Value: " << set.value << std::endl;
    os << "Included Points: ";
    for (auto edge : set.included_edges) {
        os << edge;
    }
    os << std::endl;
    os << "Excluded Points: ";
    for (auto edge : set.excluded_edges) {
        os << edge;
    }
    os << std::endl;
    os << "Level: " << set.level << std::endl;
    os << "AP primal solvution: ";
    for (size_t i = 0; i < set.ap_solution.primal.size(); ++i) {
        os << "(" << i << ", " << set.ap_solution.primal[i] << ") ";
    }
    os << std::endl;
    return os;
}

