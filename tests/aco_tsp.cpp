// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <fstream>
#include <UnitTest++.h>

#include <tsp/common/data_loader.h>
#include <heuristics/aco/tsp/aco.h>

namespace {
SUITE(AcoTspTest) {
TEST(ATSP_FTV33) {
    std::ifstream ifs("data/ftv33.atsp");
    std::vector<value_type> matrix;
    size_t dimension;
    tsp::load_tsplib_problem(ifs, matrix, dimension);
    const value_type expected_solution = 1286;

    tsp::InitialData data(matrix, dimension);
    data.parameters["iterations"] = "300";
    data.parameters["alpha"] = "1";
    data.parameters["beta"] = "1";
    data.parameters["p"] = "0.2";
    data.parameters["q"] = "1300";
    data.parameters["candidate_list_factor"] = "0.25";

    auto solution = aco_tsp::solve(data);
    CHECK_CLOSE(expected_solution, solution.value, 100);
}

TEST(STSP_GR24) {
    std::ifstream ifs("data/stsp/gr24.tsp");
    std::vector<value_type> matrix;
    size_t dimension;
    tsp::load_tsplib_problem(ifs, matrix, dimension);
    const value_type expected_solution = 1272;

    tsp::InitialData data(matrix, dimension);
    data.parameters["iterations"] = "200";
    data.parameters["alpha"] = "1";
    data.parameters["beta"] = "1";
    data.parameters["p"] = "0.2";
    data.parameters["q"] = "1300";
    data.parameters["candidate_list_factor"] = "0.25";

    auto solution = aco_tsp::solve(data);
    CHECK_CLOSE(expected_solution, solution.value, 100);
}
}
}  // namespace
