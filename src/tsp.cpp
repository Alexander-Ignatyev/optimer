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
    node->data.is_right = false;
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

void TspSolver::branch(const Node<Set> *node, value_type &record,
    std::vector<Node<Set> *> &nodes, Solution &sol, Stats &stats) {

    stats.branches++;
    Point move;
    copy_matrix(matrix_, matrix_original_, dimension_, node);
    if (!select_move(matrix_, *node, move)) {
        LOG(WARNING) << "TSP AP: Error! Cannot select move";
        dump_to_log(node);
        return;
    }

    bool append_move_right = false;

    Node<Set> *node1 = mm_->alloc(node);
    node1->data.level = node->data.level+1;
    node1->data.move = move;
    node1->data.is_right = true;
    anti_cycle(node1);
    copy_matrix(matrix_, matrix_original_, dimension_, node1);
    node1->data.value = transform_node(matrix_, node1);
    assert(mm_->CheckRefs(node));
    ++stats.sets_generated;
    if (node1->data.value < record) {
        if (node1->data.level == dimension_ - 2) {
            record = node1->data.value;
            LOG(INFO) << "ATSP AP: found new record: "
                << record << ". Level: " << node1->data.level;
            sol.route = create_tour(node1->data.ap_solve);
#ifndef NDEBUG
            if (sol.route.empty()) {
                std::cerr << "error AP solve\n";
                for (size_t i = 0; i < node1->data.ap_solve.size(); ++i) {
                    std::cout << "(" << i << ", ";
                    std::cout << node1->data.ap_solve[i] << ") --- ";
                }
                std::cout << std::endl;
                node = node1;
                while (node->parent) {
                    if (node->data.is_right) {
                        std::cout << "(" << node->data.move.x;
                        std::cout << ", " << node->data.move.y << ")\n";
                    }
                    node = const_cast<Node<Set> *>(node->parent);
                }
                print_matrix(matrix_, dimension_, std::cout);
                std::cout << std::endl;
                exit(2);
            }
#endif

            sol.value = record;
            mm_->free(node1);
            return;
        }
        append_move_right = true;
    } else {
        ++stats.sets_constrained_by_record;
    }

    Node<Set> *node2 = mm_->alloc(node);
    node2->data.level = node->data.level;
    node2->data.point = move;
    node2->data.is_right = false;
    copy_matrix(matrix_, matrix_original_, dimension_, node2);
    node2->data.value = transform_node(matrix_, node2);
    ++stats.sets_generated;
    if (node2->data.value < record) {
        nodes.push_back(node2);
    } else {
        ++stats.sets_constrained_by_record;
        mm_->free(node2);
    }

    if (append_move_right) {
        nodes.push_back(node1);
    } else {
        mm_->free(node1);
    }
}

std::vector<size_t> TspSolver::create_tour(const std::vector<size_t> &ap_sol
    , std::ostream &logger_) {
    std::vector<size_t> tour;
    size_t start = 0;
    tour.push_back(start);
    size_t finish = ap_sol[start];
    tour.push_back(finish);
    size_t counter = 0;
    while (start != finish && counter < 1000) {
        ++counter;
        finish = ap_sol[finish];
        tour.push_back(finish);
    }
    if (counter > 999) {
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
    if (pnode->parent) {
        target[pnode->data.point.x*rank+pnode->data.point.y] = M_VAL;
        while (pnode->parent) {
            if (pnode->data.is_right) {
                for (unsigned k = 0; k < rank; ++k) {
                    target[rank*pnode->data.move.x+k] = M_VAL;
                    target[rank*k+pnode->data.move.y] = M_VAL;
                }
            } else {
                target[pnode->data.point.x*rank+pnode->data.point.y] = M_VAL;
            }
            target[pnode->data.point.x*rank+pnode->data.point.y] = M_VAL;
            pnode = pnode->parent;
        }
    }
}


bool TspSolver::select_move(const value_type *data
    , const Node<Set> &node, Point &move) const {
    bool move_selected = false;
    value_type theta = 0;

    for (size_t i = 0; i < node.data.ap_solve.size(); ++i) {
        size_t j = node.data.ap_solve[i];
        value_type min_i = M_VAL;
        value_type min_j = M_VAL;
        value_type val = data[i*dimension_+j];
        if (is_m(val)) {
            continue;
        }
        for (unsigned k = 0; k < dimension_; ++k) {
            if (k != j) {
                if (min_i > data[i *dimension_+k]) {
                    min_i = data[i *dimension_+k];
                }
            }
            if (k != i) {
                if (min_j > data[k*dimension_+j]) {
                    min_j = data[k*dimension_+j];
                }
            }
        }
        if (!is_m(min_i) && !is_m(min_j)) {
            if (!move_selected || (theta < (min_i + min_j - 2*val))) {
                theta = min_i + min_j - 2*val;
                move.x = i;
                move.y = j;
                move_selected = true;
            }
        }
    }
    return move_selected;
}

value_type TspSolver::transform_node(const value_type *data, Node<Set> *node) {
    std::vector<size_t> markIndices = ap_solver_.transform(data, dimension_);
    node->data.ap_solve.resize(dimension_);
    for (size_t i = 0; i < dimension_; ++i) {
        node->data.ap_solve[markIndices[i]] = i;
    }

    value_type d0 = 0;
    for (unsigned i = 0; i < dimension_; ++i) {
        if (!is_m(data[markIndices[i]*dimension_+i])) {
            d0 += data[markIndices[i]*dimension_+i];
        }
    }
    const Node<Set> *pnode = node;
    while (pnode) {
        if (pnode->data.is_right) {
            auto index = pnode->data.move.x*dimension_+pnode->data.move.y;
            d0 += matrix_original_[index];
            node->data.ap_solve[pnode->data.move.x] = pnode->data.move.y;
        }
        pnode = pnode->parent;
    }
    return d0;
}

void TspSolver::anti_cycle(Node<Set> *node) {
    bool bContinue = true;
    size_t start, finish;
    start = node->data.move.x;
    finish = node->data.move.y;
    while (bContinue) {
        bContinue = false;
        const Node<Set> *pnode = node->parent;
        while (pnode) {
            if (pnode->data.is_right) {
                if (pnode->data.move.x == finish) {
                    finish = pnode->data.move.y;
                    bContinue = true;
                    break;
                }
                if (pnode->data.move.y == start) {
                    start = pnode->data.move.x;
                    bContinue = true;
                    break;
                }
            }
            pnode = pnode->parent;
        }
    }
    node->data.point.x = finish;
    node->data.point.y = start;
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
    oss.str(std::string());  // clear stream
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
    os << "Move: " << set.move << "; ";
    os << (set.is_right ? "right" : "left") << std::endl;
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

