// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include <vector>

#include <UnitTest++.h>
#undef CHECK

#include <tsp/symmetric/lagrangean.h>

namespace {
    TEST(stsp_lagrengean_relaxation) {
        std::vector<value_type> matrix = {
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
        size_t dimension = 12;

        stsp::LagrangeanRelaxation lr(dimension);
        auto solution = lr.solve(matrix, dimension, 0.9, 30.0, 100);
        CHECK_CLOSE(1760.0, static_cast<double>(solution.value), 0.001);
    }
}  // namespace
