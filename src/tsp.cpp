// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "tsp.h"

#include <cstring>

#include <algorithm>
#include <vector>
#include <set>
#include <unordered_set>

#include <g2log.h>

#include "stats.h"

namespace {
bool is_m(int val) {
    return val >= M_VAL;
}

bool is_m(double val) {
    return val >= M_VAL;
}
}

struct PointHash {
    std::hash<decltype(TspSolver::Point::x)> hash_x;
    std::hash<decltype(TspSolver::Point::x)> hash_y;
    size_t operator()(const TspSolver::Point &point) const {
        return hash_x(point.x) ^ hash_y(point.y);
    }
};

struct PointEqualsTo {
    bool operator()(const TspSolver::Point &lhs
        , const TspSolver::Point &rhs) const {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
};

TspSolver::TspSolver()
    : dimension_(0)
    , matrix_(nullptr)
    , matrix_original_(nullptr)
    , mm_(nullptr) { }

void TspSolver::init(const TspInitialData &data, MemoryManager<Set> *mm) {
    matrix_original_ = data.matrix;
    dimension_ = data.rank;
    mm_ = mm;
    matrix_ = new value_type[dimension_*dimension_];
}

void TspSolver::get_initial_node(Node<Set> *node) {
    node->data.level = 0;
    transform_node(node);
}

void TspSolver::get_initial_solution(Solution *sol) {
    Solution tmpSol;
    sol->value = M_VAL;
    for (unsigned i = 0; i < dimension_; ++i) {
        tmpSol.value = 0;
        tmpSol.route.clear();
        get_greedy_solution(matrix_original_, dimension_, tmpSol, i);
        two_opt(&tmpSol);
        if (sol->value > tmpSol.value) {
            *sol = tmpSol;
        }
    }
    LOG(INFO) << "Initial solution: " << sol->value;
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

void TspSolver::branch(const Node<Set> *node, value_type &record,
    std::vector<Node<Set> *> &nodes, Solution &sol, Stats &stats) {

    if (node->data.value >= record) {
        ++stats.sets_constrained_by_record;
        return;
    }
    stats.branches++;
    std::vector<Point> points;
    if (!select_move(*node, &points)) {
        LOG(WARNING) << "TSP AP: Error! Cannot select move";
        dump_to_log(node);
        CHECK(false);
        return;
    }

    std::vector<Point> included_points;
    for (auto point : points) {
        Node<Set> *new_node = mm_->alloc(node);
        new_node->data.level = node->data.level+1;
        new_node->data.included_points = included_points;
        new_node->data.excluded_points.push_back(point);
        transform_node(new_node);
        included_points.push_back(point);
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
            mm_->free(new_node);
            continue;
        }

        if (new_node->data.value < record) {
            nodes.push_back(new_node);
        } else {
            if (is_m(new_node->data.value)) {
                LOG(INFO) << "branch: sets_constrained_by_record with M_VAL";
            }
            ++stats.sets_constrained_by_record;
            mm_->free(new_node);
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


void TspSolver::get_greedy_solution(const value_type *data, size_t dimension
    , Solution &sol, unsigned startPoint) {
    sol.value = 0;

    std::set<size_t> points;
    for (size_t i = 0; i < dimension; ++i)
        points.insert(i);
    size_t currPoint = startPoint;
    size_t minPoint;
    value_type minValue = M_VAL + 10;
    std::set<size_t>::iterator pos;

    points.erase(currPoint);
    sol.route.push_back(currPoint);
    while (!points.empty()) {
        minValue = M_VAL + 10;
        for (pos = points.begin(); pos != points.end(); ++pos) {
            if (data[currPoint*dimension+*pos] < minValue) {
                minValue = data[currPoint*dimension+*pos];
                minPoint = *pos;
            }
        }
        sol.value += minValue;
        points.erase(minPoint);
         currPoint = minPoint;
        sol.route.push_back(currPoint);
    }
    sol.value += data[currPoint*dimension+startPoint];
    sol.route.push_back(startPoint);
}

bool TspSolver::two_opt(Solution *sol) const {
    bool bResult = false;
    const value_type *mtx = matrix_original_;
    bool bContinue = true;

    while (bContinue) {
        bContinue = false;
        auto &route = sol->route;
        for (unsigned first = 1, second = 2; second < route.size()
                - 2; ++first, ++second) {
            value_type delta = (mtx[route[first - 1]*dimension_+route[first]]
                + mtx[route[first]*dimension_+route[second]]
                + mtx[route[second]*dimension_+route[second + 1]])
                - (mtx[route[first- 1]*dimension_+route[second]]
                + mtx[route[second]*dimension_+route[first]]
                + mtx[route[first]*dimension_+route[second + 1]]);
            if (delta > 0) {
                std::swap(route[first], route[second]);
                sol->value -= delta;
                bResult = true;
                bContinue = true;
            }
        }
    }
    return bResult;
}

bool TspSolver::select_move(const Node<Set> &node
    , std::vector<Point> *moves) const {
    std::vector<char> selected_vertices(dimension_, 0);
    std::unordered_set<Point, PointHash, PointEqualsTo> included_points;
    const Node<Set> *tmp_node = &node;
    while (tmp_node->parent) {
        included_points.insert(
            tmp_node->data.included_points.begin()
            , tmp_node->data.included_points.end());
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
            Point point = {start, finish};
            if (included_points.find(point) == included_points.end()) {
                moves->push_back(point);
            }
            start = finish;
            finish = ap_solution[finish];
        }
        moves->push_back({start, finish});
    }
    return !moves->empty();
}

void TspSolver::transform_node(Node<Set> *node) {
    // restore included and excluded points from branch
    std::vector<Point> included_points;
    std::vector<Point> excluded_points;
    const Node<Set> *tmp_node = node;
    while (tmp_node->parent) {
        included_points.insert(included_points.end()
            , tmp_node->data.included_points.begin()
            , tmp_node->data.included_points.end());
        excluded_points.insert(excluded_points.end()
            , tmp_node->data.excluded_points.begin()
            , tmp_node->data.excluded_points.end());
        tmp_node = tmp_node->parent;
    }

    std::vector<size_t> i_original_to_new(dimension_, 0);
    std::vector<size_t> j_original_to_new(dimension_, 0);
    for (const Point &point : included_points) {
        i_original_to_new[point.x] = dimension_;
        j_original_to_new[point.y] = dimension_;
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

    for (const Point &point : excluded_points) {
        if (i_original_to_new[point.x] == dimension_
            || j_original_to_new[point.y] == dimension_) {
                continue;
            }
        size_t index = i_original_to_new[point.x] * dimension_new;
        index += j_original_to_new[point.y];
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
        for (auto point : node->data.excluded_points) {
            i_new = i_original_to_new[point.x];
            j_new = j_original_to_new[point.y];
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
    for (auto point : included_points) {
        ap_primal_original[point.x] = point.y;
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
    , const Node<Set> *node) {
    if (route.empty()) {
        dump_to_log(node);
        LOG(FATAL) << "Cannot build a tour. Error AP solve\n";
    }
}

void TspSolver::dump_to_log(const Node<Set> *node) {
    std::ostringstream oss;
    oss << node->data;
    print_matrix(matrix_original_, dimension_, oss);
    LOG(WARNING) << oss.str();
    oss.str(std::string());
    if (node->parent != nullptr) {
        dump_to_log(node->parent);
    }
}

std::ostream &operator<<(std::ostream &os, const TspSolver::Point &point) {
    os << "(" << point.x << ", " << point.y << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const TspSolver::Set &set) {
    os << "<< Set: " << std::endl;
    os << "Value: " << set.value << std::endl;
    os << "Included Points: ";
    for (auto point : set.included_points) {
        os << point;
    }
    os << std::endl;
    os << "Excluded Points: ";
    for (auto point : set.excluded_points) {
        os << point;
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

