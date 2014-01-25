// Copyright (c) 2008-2014 Alexander Ignatyev. All rights reserved.

#ifndef HEURISTICS_ACO_TSP_PARAMS_H_
#define HEURISTICS_ACO_TSP_PARAMS_H_

#include <string>
#include <map>

namespace aco_tsp {
struct Params {
    Params();
    void init(const std::map<std::string, std::string> &parameters);
    double alpha;
    double beta;
    double p;
    double q;
    double candidate_list_factor;
    size_t iterations;
};
}  // namespace aco_tsp
#endif  // HEURISTICS_ACO_TSP_PARAMS_H_
