// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef BNB_SERIAL_BNB_H_
#define BNB_SERIAL_BNB_H_

#include <stack>
#include <vector>
#include <iostream>

#include <common/timer.h>
#include <common/signals.h>

#include "defs.h"
#include "bnb.h"
#include "tree.h"
#include "stats.h"

namespace bnb {
template <typename Solver, typename NodesContainer = LifoContainer>
class SerialBNB {
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

#include "serial_bnb-inl.h"

#endif  // BNB_SERIAL_BNB_H_
