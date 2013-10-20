// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_ASYMMETRIC_TSP_H_
#define TSP_ASYMMETRIC_TSP_H_

#include <vector>

#include <bnb/defs.h>
#include <bnb/tree.h>

#include <tsp/common/types.h>

#include "ap_solver.h"

using bnb::Node;

namespace bnb {
struct Stats;
}  // namespace bnb

class TspSolver {
 public:
    typedef tsp::InitialData InitialData;
    typedef tsp::Solution Solution;
    struct Point {
        size_t x;
        size_t y;
    };

    struct Set {
        value_type value;
        std::vector<Point> included_points;
        std::vector<Point> excluded_points;
        ApSolver<value_type>::Solution ap_solution;
        unsigned level;
        bool operator < (const Set &other) const {
            return value > other.value;
        }
        void clear() {
            included_points.clear();
            excluded_points.clear();
            ap_solution.clear();
        }
    };

    /*struct Solution {
        value_type value;
        std::vector<size_t> route;
    };*/

    TspSolver();

    // mandatory function
    void init(const tsp::InitialData &data, bnb::SearchTree<Set> *mm);
    void get_initial_node(Node<Set> *node);
    void get_initial_solution(Solution *sol);
    void branch(const Node<Set> *node, value_type &record
       , std::vector<Node<Set> *> &nodes, Solution &sol, bnb::Stats &stats);

 private:
    static std::vector<size_t> create_tour(const std::vector<size_t> &ap_sol);
    static void get_greedy_solution(const value_type *data, size_t rank
        , Solution &sol, unsigned startPoint);
    bool two_opt(Solution *sol) const;
    bool select_move(const Node<Set> &node, std::vector<Point> *moves) const;
    void transform_node(Node<Set> *node);
    static void print_matrix(const value_type *matrix, size_t rank
        , std::ostream &logger = std::cout);
    void check_route(const decltype(Solution::route) &route
        , const Node<Set> *node);
    void dump_to_log(const Node<Set> *node);

    size_t dimension_;
    value_type *matrix_;
    const value_type *matrix_original_;

    bnb::SearchTree<Set> *search_tree_;
    ApSolver<value_type> ap_solver_;
};

std::ostream &operator<<(std::ostream &os, const TspSolver::Point &point);
std::ostream &operator<<(std::ostream &os, const TspSolver::Set &set);

#endif  // TSP_ASYMMETRIC_TSP_H_
