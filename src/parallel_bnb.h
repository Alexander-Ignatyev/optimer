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
#include "bnb.hpp"
#include "tree.hpp"
#include "stats.h"
#include "timer.hpp"
#include "load_balancer.h"

template <typename SolverFactory, typename NodesContainer = LifoContainer>
class ParallelBNB {
    typedef typename SolverFactory::Solver Solver;
    typedef typename Solver::Set Set;
    typedef typename Solver::Solution Solution;
    typedef typename Solver::InitialData InitialData;
    typedef std::vector<Node<Set> *> ListNodes;

 public:
    explicit ParallelBNB(const LoadBalancerParams &params)
        : balancer_params_(params)
        , record_(0)
        , num_working_threads_(0) { }

    Solution solve(const InitialData &data
        , size_t max_branches = -1
        , value_type record = M_VAL);

    void print_stats(std::ostream &os) const;

 private:
     const LoadBalancerParams &balancer_params_;

     const InitialData *initial_data_;

     volatile value_type record_;
     std::mutex mutex_record_;

     volatile unsigned num_working_threads_;
     std::queue<Node<Set> *> queue_sets_;
     std::mutex mutex_sets_;
     std::condition_variable condvar_sets_;

     std::vector<ListNodes> list_nodes_;
     std::vector<Stats> list_stats_;
     Stats initial_stats_;
     MemoryManager<Set> mm_;

     void start(unsigned threadID);
};

#include "parallel_bnb-inl.h"

#endif  // SRC_PARALLEL_BNB_H_
