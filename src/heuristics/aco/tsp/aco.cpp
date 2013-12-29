// Copyright (c) 2008-2013 Alexander Ignatyev. All rights reserved.

#include "aco.h"

#include <algorithm>
#include <sstream>

#include <common/log.h>

#include <common/signals.h>

#include "ant.h"
#include "params.h"

namespace aco_tsp {
void update_pheromone(const Params &params
                      , const std::vector<tsp::Solution> &solutions
                      , std::vector<value_type> &matrix_theta
                      , size_t dimension);

tsp::Solution solve(const tsp::InitialData &data) {
    Signals::InterruptingSignalGuard signalGuard;

    size_t dimension = data.rank;
    std::vector<value_type> matrix_dist = data.matrix;
    std::vector<value_type> matrix_theta(dimension*dimension, 1);
    LOG(INFO) << "ACO TSP starting:";
    LOG(INFO) << "dimension = " << dimension;
    Params params;
    params.init(data.parameters);

    for (size_t i = 0; i < dimension; ++i) {
        for (size_t j = 0; j < dimension; ++j) {
            size_t index = i*dimension+j;
            matrix_theta[index] = matrix_dist[index]*(1.0-params.p) + 0.01;
        }
    }

    std::vector<size_t> points(dimension);
    for (size_t i = 0; i < dimension; ++i) {
        points[i] = i;
    }

    std::random_shuffle(points.begin(), points.end());
    std::vector<Ant> ants(dimension/2);
    for (size_t k = 0; k < ants.size(); ++k) {
        ants[k].init(matrix_dist, matrix_theta, dimension, params, points[k]);
    }

    std::vector<tsp::Solution> solutions(ants.size());
    tsp::Solution best_solution;
    best_solution.value = M_VAL;
    for (size_t n = 0
         ; n < params.iterations && !Signals::is_interrupted()
         ; ++n) {
        for (size_t k = 0; k < ants.size(); ++k) {
            solutions[k] = ants[k].solve();
        }

        for (size_t k = 0; k < solutions.size(); ++k) {
            if (solutions[k] < best_solution) {
                best_solution = solutions[k];
                LOG(INFO) << "Record: " << best_solution.value
                    << " on iter: " << n;
                std::ostringstream oss;
                best_solution.write_as_json(oss);
                LOG(INFO) << oss.str();
            }
        }

        update_pheromone(params, solutions, matrix_theta, dimension);
    }
    return best_solution;
}

void update_pheromone(const Params &params
                      , const std::vector<tsp::Solution> &solutions
                      , std::vector<value_type> &matrix_theta
                      , size_t dimension) {
    for (size_t n = 0; n < matrix_theta.size(); ++n) {
        matrix_theta[n] = matrix_theta[n]*(1-params.p) + 0.01;
    }

    for (unsigned k = 0; k < solutions.size(); ++k) {
        value_type theta = params.q / solutions[k].value;
        const std::vector<size_t> &route = solutions[k].route;
        for (unsigned i = 0; i < route.size() - 1; ++i) {
            matrix_theta[route[i] * dimension + route[i + 1]] += theta;
        }
    }
}

}  // namespace aco_tsp
