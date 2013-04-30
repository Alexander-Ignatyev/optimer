// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_TSP_H_
#define SRC_TSP_H_

#include <vector>

#include "defs.h"
#include "ap_solver.h"
#include "tree.h"

struct Stats;

struct TspInitialData {
    TspInitialData(value_type *m, size_t r): matrix(m), rank(r) {
    }

    const value_type *matrix;
    size_t rank;
};

class TspSolver {
 public:
    typedef TspInitialData InitialData;
    struct Point {
        size_t x;
        size_t y;
    };

    struct Set {
        Point move;
        Point point;
        bool is_right;
        value_type value;
        std::vector<size_t> ap_solve;
        unsigned level;
        bool operator < (const Set &other) const {
            return value > other.value;
        }
    };

    struct Solution {
        value_type value;
        std::vector<size_t> route;
    };

    TspSolver();

    // mandatory function
    void init(const TspInitialData &data, MemoryManager<Set> *mm);
    void get_initial_node(Node<Set> *node);
    void get_initial_solution(Solution *sol);
    void branch(const Node<Set> *node, value_type &record
       , std::vector<Node<Set> *> &nodes, Solution &sol, Stats &stats);


 private:
    static std::vector<size_t> create_tour(const std::vector<size_t> &ap_sol
        , std::ostream &logger = std::cout);
    static void get_greedy_solution(const value_type *data, size_t rank
        , Solution &sol, unsigned startPoint);
    static void copy_matrix(value_type *target, const value_type *source
        , size_t rank, const Node<Set> *pnode);
    static void anti_cycle(Node<Set> *node);
    bool two_opt(Solution *sol) const;
    bool select_move(const value_type *data
        , const Node<Set> &node, Point &move) const;
    value_type transform_node(const value_type *data, Node<Set> *node);
    static void print_matrix(const value_type *matrix, size_t rank
        , std::ostream &logger = std::cout);
    void check_route(const decltype(Solution::route) &route
        , const Node<Set> *node);
    void dump_to_log(const Node<Set> *node);

    size_t dimension_;
    value_type *matrix_;
    const value_type *matrix_original_;

    MemoryManager<Set> *mm_;
    APSolver<value_type> ap_solver_;
};

std::ostream &operator<<(std::ostream &os, const TspSolver::Point &point);
std::ostream &operator<<(std::ostream &os, const TspSolver::Set &set);

#endif  // SRC_TSP_H_
