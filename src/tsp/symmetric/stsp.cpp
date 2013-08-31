// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "stsp.h"

#include <g2log.h>
#include <bnb/stats.h>
#include <tsp/symmetric/ms_one_tree.h>

namespace stsp {
    ClassicalSolver::ClassicalSolver()
        : dimension_(0)
        , matrix_(nullptr)
        , matrix_original_(nullptr)
        , search_tree_(nullptr) {
    }

    ClassicalSolver::~ClassicalSolver() {
        delete [] matrix_;
    }

    void ClassicalSolver::init(const InitialData &data
                               , BnbSearchTree<Set> *mm) {
        matrix_original_ = data.matrix;
        dimension_ = data.rank;
        search_tree_ = mm;
        delete [] matrix_;
        matrix_ = new value_type[dimension_*dimension_];
    }
    void ClassicalSolver::get_initial_node(Node<Set> *node) {
        node->data.level = 0;
        transform_node(node);
    }

    void ClassicalSolver::get_initial_solution(Solution *sol) {
        sol->value = M_VAL;
    }

    void ClassicalSolver::branch(const Node<Set> *node, value_type &record
         , std::vector<Node<Set> *> &nodes, Solution &sol, Stats &stats) {
        if (node->data.value > record) {
            ++stats.sets_constrained_by_record;
        }
        ++stats.branches;

        auto moves = select_moves(node);
        CHECK(!moves.empty()) << "Cannot select moves";
        for (auto move : moves) {
            ++stats.sets_generated;
            auto child = search_tree_->create_node(node);
            child->data.level = node->data.level+1;
            child->data.excluded_points.push_back(move);
            transform_node(child);
            if (child->data.value >= record) {
                ++stats.sets_constrained_by_record;
                search_tree_->release_node(child);
            } else {
                if (build_solution(child, &sol)) {
                    record = sol.value;
                    search_tree_->release_node(child);
                } else {
                    nodes.push_back(child);
                }
            }
        }
    }

    void ClassicalSolver::transform_node(Node<Set> *node) {
        // restore excluded points from branch
        memcpy(matrix_, matrix_original_
               , dimension_*dimension_*sizeof(matrix_original_[0]));
        const Node<Set> *tmp_node = node;
        while (tmp_node->parent) {
            for (const Point &point : tmp_node->data.excluded_points) {
                matrix_[point.x*dimension_+point.y] = M_VAL;
                matrix_[point.y*dimension_+point.x] = M_VAL;
            }
            tmp_node = tmp_node->parent;
        }

        auto solution = MSOneTree::solve(matrix_, dimension_);
        node->data.ms1_solution = std::move(solution.edges);
        node->data.value = solution.value;
    }

    std::vector<ClassicalSolver::Point>
    ClassicalSolver::select_moves(const Node<Set> *node) {
        std::vector<Point> moves;

        std::vector<size_t> degrees(dimension_, 0);
        for (auto &point : node->data.ms1_solution) {
            ++degrees[point.first];
            ++degrees[point.second];
        }

        static const size_t BORDER_VALUE = 2;
        size_t selected_vertex = 0;
        size_t selected_degree = degrees.size();
        for (size_t i = 0; i < degrees.size(); ++i) {
            size_t degree = degrees[i];
            if (degree > BORDER_VALUE && selected_degree > degree) {
                selected_degree = degree;
                selected_vertex = i;
            }
        }

        for (auto &point : node->data.ms1_solution) {
            if (point.first == selected_vertex
                || point.second == selected_vertex) {
                moves.push_back(point);
            }
        }

        return moves;
    }

    bool ClassicalSolver::build_solution(const Node<Set> *node
                                         , Solution *solution) {
        static const size_t BORDER_VALUE = 2;
        std::vector<size_t> degrees(dimension_, 0);
        for (auto &point : node->data.ms1_solution) {
            ++degrees[point.first];
            ++degrees[point.second];
        }
        for (size_t i = 0; i < degrees.size(); ++i) {
            size_t degree = degrees[i];
            if (degree != BORDER_VALUE) {
                return false;
            }
        }

        LOG(DEBUG) << "Found solution " << node->data.value;
        solution->value = node->data.value;
        solution->route.clear();
        solution->route.resize(dimension_, dimension_);
        std::vector<size_t> distances(dimension_, dimension_);
        for (auto &point : node->data.ms1_solution) {
            if (solution->route[point.first] == dimension_
                    && distances[point.second] == dimension_) {
                solution->route[point.first] = point.second;
                distances[point.second] = point.first;
            } else if (solution->route[point.second] == dimension_
                    && distances[point.first] == dimension_) {
                solution->route[point.second] = point.first;
                distances[point.first] = point.second;
            } else {
                CHECK(false) << "Incorrect solution's route";
            }
        }
        return true;
    }
}  // namespace stsp
