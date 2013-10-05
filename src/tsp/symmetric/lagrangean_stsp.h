// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_SYMMETRIC_LAGRANGEAN_STSP_H_
#define TSP_SYMMETRIC_LAGRANGEAN_STSP_H_

#include <bnb/tree.h>
#include <tsp/symmetric/common.h>
#include <tsp/symmetric/lagrangean.h>

struct Stats;

namespace stsp {
    class LagrangeanSolver {
    public:
        typedef stsp::Point Point;
        typedef stsp::Set Set;
        typedef stsp::Solution Solution;
        typedef stsp::InitialData InitialData;

        LagrangeanSolver();

        void init(const InitialData &data, BnbSearchTree<Set> *mm);
        void get_initial_node(Node<Set> *node);
        void get_initial_solution(Solution *sol);
        void branch(const Node<Set> *node, value_type &record
                , std::vector<Node<Set> *> &nodes, Solution &sol, Stats &stats);

    private:
        void transform_node(Node<Set> *node, value_type record, Stats &stats);
        std::vector<Point> select_moves(const Node<Set> *node);
        bool build_solution(const Node<Set> *node, Solution *solution);

        size_t dimension_;
        std::vector<value_type> matrix_;
        std::vector<value_type> matrix_original_;

        BnbSearchTree<Set> *search_tree_;
        LagrangeanRelaxation lr_;
        Solution solution_initial_;

        size_t gradient_max_iters_;
        value_type epsilon_;
    };
}  // namespace stsp


#endif  // TSP_SYMMETRIC_LAGRANGEAN_STSP_H_
