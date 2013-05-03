// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_SERIAL_BNB_H_
#define SRC_SERIAL_BNB_H_

#include <stack>
#include <vector>
#include <iostream>

#include "defs.h"
#include "bnb.h"
#include "tree.h"
#include "timer.h"
#include "stats.h"

template <typename Solver, typename NodesContainer = LifoContainer>
class SerialBNB {
    typedef typename Solver::Set Set;
    typedef typename Solver::Solution Solution;
    typedef typename Solver::InitialData InitialData;

 public:
    Solution solve(const InitialData &data
        , size_t max_branches = -1, value_type record = M_VAL);

    void print_stats(std::ostream &os) const {
        std::cout << "\nStats: \n" << stats_ << std::endl;
    }

 private:
    Stats stats_;
};

#include "serial_bnb-inl.h"

#endif  // SRC_SERIAL_BNB_H_
