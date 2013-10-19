// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef BNB_REQUESTING_SCHEDULER_H_
#define BNB_REQUESTING_SCHEDULER_H_

#include <atomic>
#include <thread>
#include <condition_variable>

#include <g2log.h>

#include "tree.h"
#include "scheduler_common.h"

namespace bnb {
struct RequestingSchedulerParams {
    unsigned num_threads;
    unsigned num_minimum_nodes;
};

template <class Set>
class RequestingScheduler {
 public:
    explicit RequestingScheduler(const RequestingSchedulerParams &params)
        : params_(params)
        , num_waiting_threads_(0) {}

    RequestingScheduler(const RequestingScheduler &scheduler)
        : params_(scheduler.params_)
        , num_waiting_threads_(0) {
    }

    ~RequestingScheduler() {
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
        if (num_waiting_threads_.load() > 0) {
            std::lock_guard<std::mutex> lock(mutex_sets_);
            while (nodes->size() > params_.num_minimum_nodes) {
                queue_sets_.push(nodes->top());
                nodes->pop();
                ++stats.sets_sent;
            }
            condvar_sets_.notify_all();
        }

        if (nodes->empty()) {
            ++num_waiting_threads_;
            std::unique_lock<std::mutex> lock(mutex_sets_);
            condvar_sets_.wait(lock,
                [this] {
                    return (!this->queue_sets_.empty())
                    || (this->num_waiting_threads_.load()
                        >= params_.num_threads);
                });
            while (nodes->size() < params_.num_minimum_nodes
                && !queue_sets_.empty()) {
                nodes->push(queue_sets_.front());
                queue_sets_.pop();
                ++stats.sets_received;
            }
            if (nodes->empty()
                    && num_waiting_threads_.load() >= params_.num_threads) {
                condvar_sets_.notify_all();
                return stats;
            }
            --num_waiting_threads_;
        }
        return stats;
    }

 private:
    RequestingSchedulerParams params_;
    std::atomic<int> num_waiting_threads_;
    std::queue<Node<Set> *> queue_sets_;
    std::mutex mutex_sets_;
    std::condition_variable condvar_sets_;
};
}  // namespace bnb
#endif  // BNB_REQUESTING_SCHEDULER_H_
