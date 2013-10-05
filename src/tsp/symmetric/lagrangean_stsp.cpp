// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "lagrangean_stsp.h"

#include <cstring>
#include <stdexcept>

#include <tuple>

#include <g2log.h>
#include <bnb/stats.h>

namespace stsp {
    double string_to_double(const std::string &str, double def_val = 0.0) {
        double result = def_val;
        try {
            result = std::stod(str);
        } catch(std::exception &ex) {
            LOG(DEBUG) << "Cannot convert '" << str << "' to double:"
            << ex.what();
        }
        return result;
    }

    double string_to_size_t(const std::string &str, size_t def_val = 0.0) {
        size_t result = def_val;
        try {
            result = std::stoul(str);
        } catch(std::exception &ex) {
            LOG(DEBUG) << "Cannot convert '" << str << "' to size_t:"
                << ex.what();
        }
        return result;
    }

    LagrangeanSolver::LagrangeanSolver()
    : dimension_(0)
    , search_tree_(nullptr)
    , gradient_max_iters_(100)
    , epsilon_(0) {
    }

    void LagrangeanSolver::init(const InitialData &data
                               , BnbSearchTree<Set> *mm) {
        matrix_original_ = data.matrix;
        dimension_ = data.rank;
        search_tree_ = mm;
        matrix_.resize(dimension_*dimension_);
        solution_initial_ = get_greedy_solution(matrix_original_, dimension_);
        LOG(INFO) << "Initial solution: " << solution_initial_.value;

        auto pos = data.parameters.find("epsilon");
        if (pos != data.parameters.end()) {
            double epsilon = string_to_double(pos->second, epsilon_);
            epsilon_ = static_cast<value_type>(epsilon);
        }

        pos = data.parameters.find("gradinet_max_iters");
        if (pos != data.parameters.end()) {
            gradient_max_iters_ = string_to_size_t(pos->second
                                    , gradient_max_iters_);
        }
    }
    void LagrangeanSolver::get_initial_node(Node<Set> *node) {
        node->data.level = 0;
        Stats stats;
        transform_node(node, solution_initial_.value, stats);
    }

    void LagrangeanSolver::get_initial_solution(Solution *sol) {
        *sol = solution_initial_;
    }

    void LagrangeanSolver::branch(const Node<Set> *node, value_type &record
            , std::vector<Node<Set> *> &nodes, Solution &sol, Stats &stats) {
        if (node->data.value+epsilon_ >= record) {
            ++stats.sets_constrained_by_record;
            return;
        }
        ++stats.branches;

        auto moves = select_moves(node);
        CHECK(!moves.empty()) << "Cannot select moves";
        for (auto move : moves) {
            ++stats.sets_generated;
            auto child = search_tree_->create_node(node);
            child->data.level = node->data.level+1;
            child->data.excluded_points.push_back(move);
            transform_node(child, record, stats);
            if (child->data.value+epsilon_ >= record) {
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

    void LagrangeanSolver::transform_node(Node<Set> *node
                , value_type record, Stats &stats) {
        // restore excluded points from branch
        memcpy(matrix_.data(), matrix_original_.data()
               , dimension_*dimension_*sizeof(matrix_original_[0]));
        const Node<Set> *tmp_node = node;
        while (tmp_node->parent) {
            for (const Point &point : tmp_node->data.excluded_points) {
                matrix_[point.x*dimension_+point.y] = M_VAL;
                matrix_[point.y*dimension_+point.x] = M_VAL;
            }
            tmp_node = tmp_node->parent;
        }

        auto solution = lr_.solve(matrix_, dimension_, record
                    , epsilon_, gradient_max_iters_);

        node->data.ms1_solution = std::move(solution.first.edges);
        node->data.value = solution.first.value;
        stats.bound_problems_solved += solution.second;
    }

    std::vector<LagrangeanSolver::Point>
    LagrangeanSolver::select_moves(const Node<Set> *node) {
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

    bool LagrangeanSolver::build_solution(const Node<Set> *node
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
