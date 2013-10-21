// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_SYMMETRIC_STSP_H_
#define TSP_SYMMETRIC_STSP_H_

#include <vector>

#include <bnb/defs.h>
#include <bnb/tree.h>
#include <tsp/common/types.h>

namespace bnb {
struct Stats;
}  // namespace bnb

using bnb::Node;

namespace stsp {
class ClassicalSolver {
 public:
    typedef tsp::Set Set;
    typedef tsp::Solution Solution;
    typedef tsp::InitialData InitialData;

    ClassicalSolver();
    ~ClassicalSolver();

    void init(const InitialData &data, bnb::SearchTree<Set> *mm);
    void get_initial_node(Node<Set> *node);
    void get_initial_solution(Solution *sol);
    void branch(const Node<Set> *node, value_type &record
           , std::vector<Node<Set> *> &nodes, Solution &sol, bnb::Stats &stats);

 private:
    void transform_node(Node<Set> *node);
    std::vector<tsp::Edge> select_moves(const Node<Set> *node);
    bool build_solution(const Node<Set> *node, Solution *solution);

    size_t dimension_;
    value_type *matrix_;
    const value_type *matrix_original_;

    bnb::SearchTree<Set> *search_tree_;
};
}  // namespace stsp


#endif  // TSP_SYMMETRIC_STSP_H_
