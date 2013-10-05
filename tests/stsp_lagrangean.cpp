// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <vector>
#include <memory>
#include <fstream>

#include <UnitTest++.h>
#undef CHECK

#include <bnb/serial_bnb.h>
#include <tsp/common/data_loader.h>
#include <tsp/symmetric/lagrangean.h>
#include <tsp/symmetric/lagrangean_stsp.h>

namespace {
    struct Nasini12Fuxture {
        Nasini12Fuxture()
        : dimension(12)
        , solution_value(1760) {
            matrix =  {
                99999, 750, 431, 402, 133, 489, 310, 566, 302, 214, 785, 762,
                750, 99999, 736, 671, 85, 174, 870, 927, 683, 336, 882, 150,
                431, 736, 99999, 117, 934, 65, 939, 305, 422, 291, 88, 507,
                402, 671, 117, 99999, 982, 727, 911, 870, 380, 754, 367, 580,
                133, 85, 934, 982, 99999, 956, 834, 118, 795, 633, 447, 446,
                489, 174, 65, 727, 956, 99999, 322, 397, 356, 336, 27, 872,
                310, 870, 939, 911, 834, 322, 99999, 127, 262, 821, 776, 81,
                566, 927, 305, 870, 118, 397, 127, 99999, 257, 429, 320, 524,
                302, 683, 422, 380, 795, 356, 262, 257, 99999, 555, 244, 290,
                214, 336, 291, 754, 633, 336, 821, 429, 555, 99999, 508, 77,
                785, 882, 88, 367, 447, 27, 776, 320, 244, 508, 99999, 293,
                762, 150, 507, 580, 446, 872, 81, 524, 290, 77, 293, 99999
            };
            initial_data.reset(new stsp::InitialData(matrix, dimension));
        }

        Nasini12Fuxture(const Nasini12Fuxture &) = delete;
        Nasini12Fuxture &operator=(Nasini12Fuxture &) = delete;

        std::vector<value_type> matrix;
        size_t dimension;
        std::shared_ptr<stsp::InitialData> initial_data;
        value_type solution_value;
    };

    struct Gr17Fixture {
        Gr17Fixture()
            : dimension(0)
            , solution_value(2085) {
            size_t dimension;
            std::ifstream ifs("data/stsp/gr17.tsp");
            TspCommon::load_tsplib_problem(ifs, matrix, dimension);
            ifs.close();

            initial_data.reset(new stsp::InitialData(matrix, dimension));
        }

        Gr17Fixture(const Gr17Fixture &) = delete;
        Gr17Fixture &operator=(Gr17Fixture &) = delete;

        std::vector<value_type> matrix;
        size_t dimension;
        std::shared_ptr<stsp::InitialData> initial_data;
        value_type solution_value;
    };

    TEST_FIXTURE(Nasini12Fuxture, stsp_lagrengean_relaxation) {
        stsp::LagrangeanRelaxation lr;
        auto solution = lr.solve(matrix, dimension, 1790, 0, 100);
        CHECK_CLOSE(solution_value
                    , static_cast<double>(solution.first.value)
                    , 0.001);
    }

    TEST_FIXTURE(Nasini12Fuxture, stsp_lr_nasini12) {
        SerialBNB<stsp::LagrangeanSolver> solver;
        auto solution = solver.solve(*initial_data);
        CHECK_CLOSE(solution_value, static_cast<double>(solution.value), 0.001);
    }

    TEST_FIXTURE(Gr17Fixture, stsp_lr_gr17) {
        SerialBNB<stsp::LagrangeanSolver> solver;
        auto solution = solver.solve(*initial_data);
        CHECK_CLOSE(solution_value, static_cast<double>(solution.value), 0.001);
    }
}  // namespace
