// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#ifndef BNB_SIMPLE_BNB_H_
#define BNB_SIMPLE_BNB_H_

#include <stack>
#include <vector>
#include <iostream>

#include <common/unix_timer.h>
#include <common/signals.h>

#include "defs.h"
#include "tree.h"
#include "stats.h"

namespace bnb {
template <typename Solver>
class SimpleBNB {
    typedef typename Solver::Set Set;
    typedef typename Solver::Solution Solution;
    typedef typename Solver::InitialData InitialData;

 public:
    Solution solve(const InitialData &data
                   , size_t max_branches = -1, value_type record = M_VAL);

    void print_stats(std::ostream &os) const;
 private:
    Stats stats_;
};
}  // namespace bnb

#include "simple_bnb-inl.h"

#endif  // BNB_SIMPLE_BNB_H_
