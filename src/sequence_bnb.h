// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_SEQUENCE_BNB_H_
#define SRC_SEQUENCE_BNB_H_

#include <stack>
#include <vector>
#include <iostream>

#include "defs.h"
#include "bnb.h"
#include "tree.h"
#include "timer.h"
#include "stats.h"

template <typename SolverProvider, typename NodesContainer = LifoContainer>
class SequenceBNB {
    typedef typename SolverProvider::Solver Solver;
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

#include "sequence_bnb-inl.h"

#endif  // SRC_SEQUENCE_BNB_H_
