// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_PARALLEL_BNB_H_
#define SRC_PARALLEL_BNB_H_

#include <thread>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>

#include "defs.h"
#include "bnb.h"
#include "tree.h"
#include "stats.h"
#include "timer.h"

template <typename Solver, typename NodesContainer, typename Scheduler>
class ParallelBNB {
    typedef typename Solver::Set Set;
    typedef typename Solver::Solution Solution;
    typedef typename Solver::InitialData InitialData;
    typedef std::vector<Node<Set> *> ListNodes;

 public:
    explicit ParallelBNB(const Scheduler &scheduler)
        : initial_data_(nullptr)
        , record_(0)
        , scheduler_(scheduler) { }

    Solution solve(const InitialData &data
        , size_t max_branches = -1
        , value_type record = M_VAL);

    void print_stats(std::ostream &os) const;

 private:
     void start(unsigned threadID);

     const InitialData *initial_data_;

     volatile value_type record_;
     std::mutex mutex_record_;

     std::vector<ListNodes> list_nodes_;
     std::vector<Stats> list_stats_;
     Stats initial_stats_;
     MemoryManager<Set> mm_;

     Scheduler scheduler_;
};

#include "parallel_bnb-inl.h"

#endif  // SRC_PARALLEL_BNB_H_
