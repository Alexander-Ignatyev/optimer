// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "tsp.h"

#include <cstring>

#include <algorithm>
#include <vector>
#include <set>

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
    node->data.value = transform_node(matrix_original_, node);
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
        return;
    }
    stats.branches++;
    std::vector<Point> points;
    copy_matrix(matrix_, matrix_original_, dimension_, node);
    if (!select_move(matrix_, *node, &points)) {
        LOG(WARNING) << "TSP AP: Error! Cannot select move";
        dump_to_log(node);
        CHECK(false);
        return;
    }

    for (auto point : points) {
        Node<Set> *new_node = mm_->alloc(node);
        new_node->data.level = node->data.level+1;
        new_node->data.point = point;

        copy_matrix(matrix_, matrix_original_, dimension_, new_node);
        new_node->data.value = transform_node(matrix_, new_node);
        ++stats.sets_generated;

        if (has_solution(new_node->data.ap_solve, dimension_)) {
            if (new_node->data.value < record) {
                record = new_node->data.value;
                LOG(INFO) << "ATSP AP: found new record: "
                    << record << ". Level: " << new_node->data.level;
                sol.route = create_tour(new_node->data.ap_solve);
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

void TspSolver::copy_matrix(value_type *target, const value_type *source
    , size_t rank, const Node<Set> *pnode) {
    memcpy(target, source, rank*rank*sizeof(value_type));
    while (pnode->parent) {
        target[pnode->data.point.x*rank+pnode->data.point.y] = M_VAL;
        pnode = pnode->parent;
    }
}


bool TspSolver::select_move(const value_type *data
    , const Node<Set> &node, std::vector<Point> *moves) const {
    std::vector<char> selected_vertices(dimension_, 0);
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
        size_t finish = node.data.ap_solve[start];
        ++selected_vertices[finish];
        for (; start != finish && path_length < dimension_; ++path_length) {
            finish = node.data.ap_solve[finish];
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
        auto finish = node.data.ap_solve[start];
        for (; selected_path_start != finish; ) {
            Point point;
            point.x = start;
            point.y = finish;
            moves->push_back(point);
            start = finish;
            finish = node.data.ap_solve[finish];
        }
        Point point;
        point.x = start;
        point.y = finish;
        moves->push_back(point);
    }
    return !moves->empty();
}

value_type TspSolver::transform_node(const value_type *data, Node<Set> *node) {
    std::vector<size_t> markIndices = ap_solver_.transform(data, dimension_);
    node->data.ap_solve.resize(dimension_);
    for (size_t i = 0; i < dimension_; ++i) {
        node->data.ap_solve[markIndices[i]] = i;
    }

    value_type value = 0;
    for (unsigned i = 0; i < dimension_; ++i) {
        if (is_m(data[markIndices[i]*dimension_+i])) {
            LOG(DEBUG) << "transform_node: invalid ap_solve";
        }
        value += data[markIndices[i]*dimension_+i];
    }
    return value;
}

void TspSolver::print_matrix(const value_type *matrix
    , size_t dimension, std::ostream &os) {
    for (size_t i = 0; i < dimension; ++i) {
        for (size_t j = 0; j < dimension; ++j) {
            if (i != j && !is_m(matrix[i*dimension+j])) {
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
    copy_matrix(matrix_, matrix_original_, dimension_, node);
    oss << "AP Solve (non-empty): ";
    for (size_t i = 0; i < node->data.ap_solve.size(); ++i) {
        size_t j = node->data.ap_solve[i];
        if (!is_m(matrix_[i*dimension_+j])) {
            oss << "(" << i << ", " << j << ") ";
        }
    }
    oss << std::endl;
    print_matrix(matrix_, dimension_, oss);
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
    os << "Point: " << set.point << std::endl;
    os << "Value: " << set.value << std::endl;
    os << "Level: " << set.level << std::endl;
    os << "AP solve: ";
    for (size_t i = 0; i < set.ap_solve.size(); ++i) {
        os << "(" << i << ", " << set.ap_solve[i] << ") ";
    }
    os << std::endl;
    return os;
}

