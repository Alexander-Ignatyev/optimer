// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef BNB_GIVING_SCHEDULER_H_
#define BNB_GIVING_SCHEDULER_H_

#include <thread>
#include <condition_variable>

#include <common/log.h>

#include "tree.h"
#include "scheduler_common.h"

namespace bnb {
struct GivingSchedulerParams {
    unsigned num_threads;
    unsigned num_minimum_nodes;
    unsigned num_maximum_nodes;
};

template <class Set>
class GivingScheduler {
 public:
    explicit GivingScheduler(const GivingSchedulerParams &params)
        : params_(params)
        , num_working_threads_(params.num_threads) {}

    GivingScheduler(const GivingScheduler &scheduler)
        : params_(scheduler.params_)
        , num_working_threads_(scheduler.params_.num_threads) {
    }

    ~GivingScheduler() {
        CHECK(queue_sets_.empty());
    }

    unsigned num_threads() const {
        return params_.num_threads;
    }

    unsigned num_minimum_nodes() const {
        return params_.num_minimum_nodes;
    }

    template <typename Container>
    SchedulerStats schedule(Container *nodes) {
        SchedulerStats stats = {0};
        if (nodes->size() > params_.num_maximum_nodes) {
            std::lock_guard<std::mutex> lock(mutex_sets_);
            while (nodes->size() > params_.num_minimum_nodes) {
                queue_sets_.push(nodes->top());
                nodes->pop();
                ++stats.sets_sent;
            }
            condvar_sets_.notify_all();
        }

        if (nodes->empty()) {
            std::unique_lock<std::mutex> lock(mutex_sets_);
            --num_working_threads_;
            condvar_sets_.wait(lock,
                [this] {
                    return (!this->queue_sets_.empty())
                    || (this->num_working_threads_ == 0);
                });
            while (nodes->size() < params_.num_minimum_nodes
                && !queue_sets_.empty()) {
                nodes->push(queue_sets_.front());
                queue_sets_.pop();
                ++stats.sets_received;
            }
            if (nodes->empty() && num_working_threads_ == 0) {
                condvar_sets_.notify_all();
                return stats;
            }
            ++num_working_threads_;
        }
        return stats;
    }

 private:
    GivingSchedulerParams params_;
    volatile unsigned num_working_threads_;
    std::queue<Node<Set> *> queue_sets_;
    std::mutex mutex_sets_;
    std::condition_variable condvar_sets_;
};
}  // namespace bnb

#endif  // BNB_GIVING_SCHEDULER_H_
