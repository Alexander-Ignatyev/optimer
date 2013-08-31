// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_SYMMETRIC_STSP_H_
#define TSP_SYMMETRIC_STSP_H_

#include <vector>

#include <bnb/defs.h>
#include <bnb/tree.h>

struct Stats;

namespace stsp {
    class ClassicalSolver {
    public:
        struct Point {
            Point(const std::pair<size_t, size_t> &pair)
                : x(pair.first)
                , y(pair.second) {}
            size_t x;
            size_t y;
        };

        struct Set {
            value_type value;
            std::vector<std::pair<size_t, size_t> > ms1_solution;
            std::vector<Point> excluded_points;
            unsigned level;
            bool operator < (const Set &other) const {
                return value > other.value;
            }
            void clear() {
                excluded_points.clear();
            }
        };


        struct Solution {
            value_type value;
            std::vector<size_t> route;
        };

        struct InitialData {
            InitialData(value_type *m, size_t r): matrix(m), rank(r) {
            }

            const value_type *matrix;
            size_t rank;
        };

        ClassicalSolver();
        ~ClassicalSolver();

        void init(const InitialData &data, BnbSearchTree<Set> *mm);
        void get_initial_node(Node<Set> *node);
        void get_initial_solution(Solution *sol);
        void branch(const Node<Set> *node, value_type &record
                , std::vector<Node<Set> *> &nodes, Solution &sol, Stats &stats);

    private:
        void transform_node(Node<Set> *node);
        std::vector<Point> select_moves(const Node<Set> *node);
        bool build_solution(const Node<Set> *node, Solution *solution);

        size_t dimension_;
        value_type *matrix_;
        const value_type *matrix_original_;

        BnbSearchTree<Set> *search_tree_;
    };
}  // namespace stsp


#endif  // TSP_SYMMETRIC_STSP_H_
