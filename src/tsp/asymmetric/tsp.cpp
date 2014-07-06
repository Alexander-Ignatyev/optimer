// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#include "tsp.h"

#include <cstring>

#include <sstream>
#include <algorithm>
#include <vector>
#include <set>

#include <common/log.h>
#include <common/triple.h>
#include <common/algo_string.h>

#include <bnb/stats.h>

namespace {
bool is_m(value_type val) {
    return val >= M_VAL;
}

}

TspSolver::TspSolver()
    : dimension_(0)
    , matrix_(nullptr)
    , matrix_original_(nullptr)
    , edge_map_(nullptr)
    , search_tree_(nullptr) { }

TspSolver::~TspSolver() {
    delete [] matrix_;
    delete [] edge_map_;
}

void TspSolver::init(const tsp::InitialData &data, bnb::SearchTree<Set> *mm) {
    matrix_original_ = &data.matrix[0];
    dimension_ = data.rank;
    search_tree_ = mm;
    matrix_ = new value_type[dimension_*dimension_];
    edge_map_ = new int[dimension_*dimension_];
    initial_solution_ = tsp::get_greedy_solution(data.matrix, data.rank);
    branching_rule = &TspSolver::branching_rule3;

    std::map<std::string, std::string>::const_iterator pos;
    pos = data.parameters.find("branching_rule");
    if (pos != data.parameters.end()) {
        size_t rule = string_to_size_t(pos->second, 1);
        switch (rule) {
            case 1:
                branching_rule = &TspSolver::branching_rule1;
                break;
            case 2:
                branching_rule = &TspSolver::branching_rule2;
                break;
            case 3:
                branching_rule = &TspSolver::branching_rule3;
                break;
            case 4:
                branching_rule = &TspSolver::branching_rule4;
                break;
            case 5:
                branching_rule = &TspSolver::branching_rule5;
                break;
        }
    }
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

    ++stats.branches;

    NodeList new_nodes = (this->*branching_rule)(node);

    // transform nodes
    for (size_t i = 0; i < new_nodes.size(); ++i) {
        Node *child = new_nodes[i];
        ++stats.sets_generated;
        transform_node(child);

        if (has_solution(child->data.ap_solution.primal, dimension_)) {
            if (child->data.value < record) {
                record = child->data.value;
                LOG(INFO) << "ATSP AP: found new record: "
                << record << ". Level: " << child->data.level;
                sol.route = create_tour(child->data.ap_solution.primal);
                sol.value = record;
                check_route(sol.route, child);
            }
            search_tree_->release_node(child);
            continue;
        }

        if (child->data.value < record) {
            nodes.push_back(child);
        } else {
            if (is_m(child->data.value)) {
                LOG(INFO) << "branch: sets_constrained_by_record with M_VAL";
            }
            ++stats.sets_constrained_by_record;
            search_tree_->release_node(child);
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

// Little, Murty, Sweeney & Carel, 1963
TspSolver::NodeList TspSolver::branching_rule1(const Node *node) {
    memset(edge_map_, 0, sizeof(edge_map_[0])*dimension_*dimension_);
    const Node *tmp_node = node;
    while (tmp_node->parent) {
        std::vector<tsp::Edge>::const_iterator pos, end;
        end = tmp_node->data.included_edges.end();
        for (pos = tmp_node->data.included_edges.begin(); pos != end; ++pos) {
            for (size_t k = 0; k < dimension_; ++k) {
                edge_map_[pos->first*dimension_ + k] = 1;
                edge_map_[k*dimension_ + pos->second] = 1;
            }
        }

        end = tmp_node->data.excluded_edges.end();
        for (pos = tmp_node->data.excluded_edges.begin(); pos != end; ++pos) {
            edge_map_[pos->first*dimension_ + pos->second] = 1;
        }
        tmp_node = tmp_node->parent;
    }

    // calculate min_values in each row and column
    typedef Triple<value_type, size_t, value_type> MinValues;
    std::vector<MinValues> col_min_values(dimension_, {M_VAL});
    std::vector<MinValues> row_min_values(dimension_, {M_VAL});
    const value_type *dual_u = &node->data.ap_solution.dual_u[0];
    const value_type *dual_v = &node->data.ap_solution.dual_v[0];
    for (size_t i = 0; i < dimension_; ++i) {
        for (size_t j = 0; j < dimension_; ++j) {
            value_type val = matrix_original_[i*dimension_+j]
                    - dual_u[i] - dual_v[j];
            if (val <= col_min_values[i].first) {
                col_min_values[i].third = col_min_values[i].first;
                col_min_values[i].first = val;
                col_min_values[i].second = j;
            } else if (val <= col_min_values[i].third) {
                col_min_values[i].third = val;
            }

            if (val <= row_min_values[i].first) {
                row_min_values[i].third = row_min_values[i].first;
                row_min_values[i].first = val;
                row_min_values[i].second = i;
            } else if (val <= col_min_values[i].third) {
                row_min_values[i].third = val;
            }
        }
    }

    value_type min_p_value = M_VAL;
    tsp::Edge edge(dimension_, dimension_);
    for (size_t i = 0; i < dimension_; ++i) {
        for (size_t j = 0; j < dimension_; ++j) {
            if (edge_map_[i*dimension_ + j] > 0) {
                continue;
            }
            value_type p_value(0);
            if (col_min_values[i].second != j) {
                p_value += col_min_values[j].first;
            } else {
                p_value += col_min_values[j].third;
            }
            if (row_min_values[j].second != i) {
                p_value += row_min_values[j].first;
            } else {
                p_value += row_min_values[j].third;
            }
            if (p_value < min_p_value) {
                min_p_value = p_value;
                edge.first = i;
                edge.second = j;
            }
        }
    }

    NodeList nodes;
    if (edge.first != dimension_) {
        Node *new_node = search_tree_->create_node(node);
        new_node->data.level = node->data.level+1;
        new_node->data.excluded_edges.push_back(edge);
        nodes.push_back(new_node);

        new_node = search_tree_->create_node(node);
        new_node->data.level = node->data.level+1;
        new_node->data.included_edges.push_back(edge);
        nodes.push_back(new_node);
    }

    return nodes;
}

// Eastman, 1958; Shapiro, 1966
TspSolver::NodeList TspSolver::branching_rule2(const Node *node) {
    NodeList nodes;
    std::vector<tsp::Edge> edges;
    select_move(*node, &edges);

    for (size_t i = 0; i < edges.size(); ++i) {
        const tsp::Edge &edge = edges[i];
        Node *new_node = search_tree_->create_node(node);
        new_node->data.level = node->data.level+1;
        new_node->data.excluded_edges.push_back(edge);
        nodes.push_back(new_node);
    }
    return nodes;
}

// Murty, 1968; Bellmore & Mallone, 1971; Smith, Srinivasan & Thomson, 1977
TspSolver::NodeList TspSolver::branching_rule3(const Node *node) {
    NodeList nodes;
    std::vector<tsp::Edge> edges;
    select_move(*node, &edges);

    std::vector<tsp::Edge> included_edges;
    for (size_t i = 0; i < edges.size(); ++i) {
        const tsp::Edge &edge = edges[i];
        Node *new_node = search_tree_->create_node(node);
        new_node->data.level = node->data.level+1;
        new_node->data.included_edges = included_edges;
        new_node->data.excluded_edges.push_back(edge);
        included_edges.push_back(edge);
        nodes.push_back(new_node);
    }
    return nodes;
}

// Bellmore & Mallone, 1971
TspSolver::NodeList TspSolver::branching_rule4(const Node *node) {
    NodeList nodes;
    const size_t *ap_solution = &node->data.ap_solution.primal[0];
    std::vector<size_t> subtour = mincard_subtour(ap_solution);
    fill_edges_map(node, edge_map_);
    const size_t length = subtour.size()-1;
    for (size_t i = 0; i < length; ++i) {
        Node *new_node = search_tree_->create_node(node);
        for (size_t j = 0; j < length; ++j) {
            if (i == j) continue;
            tsp::Edge edge(subtour[i], subtour[j]);
            if (edge_map_[edge.first*dimension_+edge.second] == 0) {
                new_node->data.excluded_edges.push_back(edge);
            }
        }
        CHECK(!new_node->data.excluded_edges.empty());
        nodes.push_back(new_node);
    }
    return nodes;
}

// Garfinkel, 1973;
TspSolver::NodeList TspSolver::branching_rule5(const Node *node) {
    NodeList nodes;
    const size_t *ap_solution = &node->data.ap_solution.primal[0];
    std::vector<size_t> subtour = mincard_subtour(ap_solution);
    fill_edges_map(node, edge_map_);

    const size_t length = subtour.size()-1;
    std::vector<int> vertices_map(dimension_, 0);
    for (size_t k = 0; k < length; ++k) {
        vertices_map[subtour[k]] = 1;
    }

    for (size_t k = 0; k < length; ++k) {
        const size_t i = subtour[k];
        Node *new_node = search_tree_->create_node(node);
        for (size_t j = 0; j < dimension_; ++j) {
            if (vertices_map[j] == 0) {
                for (size_t l = 0; l < k; ++l) {
                    if (subtour[l] == j) continue;
                    tsp::Edge edge(subtour[l], j);
                    if (edge_map_[edge.first*dimension_+edge.second] == 0) {
                        new_node->data.excluded_edges.push_back(edge);
                    }
                }
            } else if (i != j && edge_map_[i*dimension_+j] == 0) {
                new_node->data.excluded_edges.push_back(tsp::Edge(i, j));
            }
        }
        if (!new_node->data.excluded_edges.empty()) {
            nodes.push_back(new_node);
        } else {
            search_tree_->release_node(new_node);
        }
    }
    return nodes;
}

void TspSolver::fill_edges_map(const Node *node, int *map) const {
    memset(map, 0, sizeof(map[0])*dimension_*dimension_);
    while (node->parent) {
        std::vector<tsp::Edge>::const_iterator pos, end;
        end = node->data.included_edges.end();
        for (pos = node->data.included_edges.begin(); pos != end; ++pos) {
            map[pos->first*dimension_ + pos->second] = 1;
        }

        end = node->data.excluded_edges.end();
        for (pos = node->data.excluded_edges.begin(); pos != end; ++pos) {
            map[pos->first*dimension_ + pos->second] = 1;
        }
        node = node->parent;
    }
}

// min_cardinality_subtour
std::vector<size_t> TspSolver::mincard_subtour(const size_t *apsolution) const {
    std::vector<char> selected_vertices(dimension_, 0);
    std::vector<size_t> subtour;
    size_t selected_path_start = dimension_;
    size_t selected_path_length = M_VAL;
    while (selected_path_length != 2) {
        size_t start = 0;
        for (; start != dimension_ && selected_vertices[start] != 0; ++start) {}
        if (start == dimension_) {
            break;
        }
        size_t path_length = 1;
        ++selected_vertices[start];
        size_t finish = apsolution[start];
        ++selected_vertices[finish];
        for (; start != finish && path_length < dimension_; ++path_length) {
            finish = apsolution[finish];
            ++selected_vertices[finish];
        }
        if (start == finish && path_length > 1
            && path_length < selected_path_length) {
            selected_path_start = start;
            selected_path_length = path_length;
        }
    }

    if (selected_path_start < dimension_) {
        size_t start = selected_path_start;
        size_t finish = apsolution[start];
        for (; selected_path_start != finish; ) {
            subtour.push_back(start);
            start = finish;
            finish = apsolution[finish];
        }
        subtour.push_back(start);
        subtour.push_back(finish);
    }

    return subtour;
}

bool TspSolver::select_move(const Node &node
                            , std::vector<tsp::Edge> *edges) const {
    std::vector<char> selected_vertices(dimension_, 0);
    fill_edges_map(&node, edge_map_);
    const size_t *ap_solution = &node.data.ap_solution.primal[0];
    std::vector<size_t> subtour = mincard_subtour(ap_solution);
    for (size_t k = 1; k < subtour.size(); ++k) {
        tsp::Edge edge(subtour[k-1], subtour[k]);
        if (edge_map_[edge.first*dimension_ + edge.second] == 0) {
            edges->push_back(edge);
        }
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
    for (size_t i = 0; i < included_points.size(); ++i) {
        const tsp::Edge &edge = included_points[i];
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

    for (size_t i = 0; i < excluded_points.size(); ++i) {
        const tsp::Edge &edge = excluded_points[i];
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
        const ApSolver<value_type>::Solution &tmp
            = node->parent->data.ap_solution;
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
        for (size_t i = 0; i < node->data.excluded_edges.size(); ++i) {
            const tsp::Edge &edge = node->data.excluded_edges[i];
            i_new = i_original_to_new[edge.first];
            j_new = j_original_to_new[edge.second];
            CHECK(i_new < dimension_new);
            CHECK(j_new < dimension_new);
            ap_solution_new.primal[i_new] = dimension_new;
        }
    }

    ap_solver_.solve(matrix_, dimension_new, &ap_solution_new);

    const size_t *ap_primal_new = &ap_solution_new.primal[0];
    const value_type *ap_dual_u_new = &ap_solution_new.dual_u[0];
    const value_type *ap_dual_v_new = &ap_solution_new.dual_v[0];

    node->data.ap_solution.init(dimension_);

    size_t *ap_primal_original = &node->data.ap_solution.primal[0];
    value_type *ap_dual_u_original = &node->data.ap_solution.dual_u[0];
    value_type *ap_dual_v_original = &node->data.ap_solution.dual_v[0];

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
    for (size_t i = 0; i < included_points.size(); ++i) {
        const tsp::Edge &edge = included_points[i];
        ap_primal_original[edge.first] = edge.second;
    }

    // calculate value
    value_type value = 0;
    for (size_t i = 0; i < dimension_; ++i) {
        value_type cost = matrix_original_[i*dimension_+ap_primal_original[i]];
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

void TspSolver::check_route(const std::vector<size_t> &route
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
    for (size_t i = 0; i < set.included_edges.size(); ++i) {
        os << set.included_edges[i];
    }
    os << std::endl;
    os << "Excluded Points: ";
    for (size_t i = 0; i < set.excluded_edges.size(); ++i) {
        os << set.excluded_edges[i];
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

