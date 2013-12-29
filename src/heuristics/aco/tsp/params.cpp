// Copyright (c) 2008-2013 Alexander Ignatyev. All rights reserved.

#include "params.h"

#include <sstream>

#include <common/log.h>

#include <common/algo_string.h>

namespace aco_tsp {
Params::Params()
    : alpha(1.0)
    , beta(1.0)
    , p(0.2)
    , q(10000)
    , candidate_list_factor(0.25)
    , iterations(1000) {
}

void Params::init(const std::unordered_map<std::string
                  , std::string> &parameters) {
    auto pos = parameters.find("alpha");
    if (pos != parameters.end()) {
        alpha = string_to_double(pos->second, alpha);
    }

    pos = parameters.find("beta");
    if (pos != parameters.end()) {
        beta = string_to_double(pos->second, beta);
    }

    pos = parameters.find("p");
    if (pos != parameters.end()) {
        p = string_to_double(pos->second, p);
    }

    pos = parameters.find("q");
    if (pos != parameters.end()) {
        q = string_to_double(pos->second, q);
    }

    pos = parameters.find("candidate_list_factor");
    if (pos != parameters.end()) {
        candidate_list_factor =
            string_to_double(pos->second, candidate_list_factor);
    }

    pos = parameters.find("iterations");
    if (pos != parameters.end()) {
        iterations = string_to_double(pos->second, iterations);
    }

    std::ostringstream oss;
    oss << "ACO TSP Parameters:";
    oss << std::endl << "alpha = " << alpha;
    oss << std::endl << "beta = " << beta;
    oss << std::endl << "p = " << p;
    oss << std::endl << "q = " << q;
    oss << std::endl << "candidate_list_factor = " << candidate_list_factor;
    oss << std::endl << "iterations = " << iterations;
    LOG(INFO) << oss.str();
}
}  // namespace aco_tsp
