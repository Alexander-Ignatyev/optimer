// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#ifndef TSP_ASYMMETRIC_TSP_H_
#define TSP_ASYMMETRIC_TSP_H_

#include <vector>

#include <bnb/defs.h>
#include <bnb/tree.h>

#include <tsp/common/types.h>

#include "ap_solver.h"

namespace bnb {
struct Stats;
}  // namespace bnb

class TspSolver {
 public:
    typedef tsp::InitialData InitialData;
    typedef tsp::Solution Solution;

    struct Set {
        value_type value;
        std::vector<tsp::Edge> included_edges;
        std::vector<tsp::Edge> excluded_edges;
        ApSolver<value_type>::Solution ap_solution;
        unsigned level;
        bool operator < (const Set &other) const {
            return value > other.value;
        }
        void clear() {
            included_edges.clear();
            excluded_edges.clear();
            ap_solution.clear();
        }
    };

    typedef bnb::Node<Set> Node;
    typedef std::vector<Node *> NodeList;

    TspSolver();
    ~TspSolver();

    // mandatory function
    void init(const InitialData &data, bnb::SearchTree<Set> *mm);
    void get_initial_node(Node *node);
    void get_initial_solution(Solution *sol);
    void branch(const Node *node, value_type &record
       , NodeList &nodes, Solution &sol, bnb::Stats &stats);

 private:
    TspSolver(const TspSolver &);
    TspSolver &operator=(const TspSolver &);

 private:
    static std::vector<size_t> create_tour(const std::vector<size_t> &ap_sol);
    void fill_edges_map(const Node *node, int *map) const;
    std::vector<size_t> mincard_subtour(const size_t *ap_solution) const;
    bool select_move(const Node &node, std::vector<tsp::Edge> *edges) const;
    // Little, Murty, Sweeney & Carel, 1963
    NodeList branching_rule1(const Node *node);
    // Eastman, 1958; Shapiro, 1966
    NodeList branching_rule2(const Node *node);
    // Murty, 1968; Bellmore & Mallone, 1971; Smith, Srinivasan & Thomson, 1977
    NodeList branching_rule3(const Node *node);
    // Bellmore & Mallone, 1971
    NodeList branching_rule4(const Node *node);
    // Garfinkel, 1973;
    NodeList branching_rule5(const Node *node);
    void transform_node(Node *node);
    static void print_matrix(const value_type *matrix, size_t rank
        , std::ostream &logger = std::cout);
    void check_route(const std::vector<size_t> &route, const Node *node);
    void dump_to_log(const Node *node);

    typedef NodeList (TspSolver::*branching_rule_ptr) (const Node *);
    branching_rule_ptr branching_rule;

    size_t dimension_;
    value_type *matrix_;
    const value_type *matrix_original_;
    int *edge_map_;
    tsp::Solution initial_solution_;

    bnb::SearchTree<Set> *search_tree_;
    ApSolver<value_type> ap_solver_;
};

std::ostream &operator<<(std::ostream &os, const tsp::Edge &point);
std::ostream &operator<<(std::ostream &os, const TspSolver::Set &set);

#endif  // TSP_ASYMMETRIC_TSP_H_
