// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_PARALLEL_BNB_INL_H_
#define SRC_PARALLEL_BNB_INL_H_

template <typename Solver, typename NodesContainer>
typename Solver::Solution
ParallelBNB<Solver, NodesContainer>::solve(
      const InitialData &data
    , size_t max_branches
    , value_type record) {
    static const size_t MIN_RANK_VALUE = 2;

    record_ = record;
    initial_data_ = &data;

    Solution sol;
    auto nodes = make_nodes_container<Solver>(LifoContainer());
    mm_.init(data.rank*data.rank*data.rank*data.rank);
    initial_stats_.clear();
    if (data.rank > MIN_RANK_VALUE) {
        Solver solver;
        solver.init(data, &mm_);
        
        Node<Set> *node = mm_.alloc(NULL);
        solver.get_initial_node(node);
        nodes.push(node);

        Solution initSol;
        solver.get_initial_solution(&initSol);
        record_ = initSol.value;

        std::vector<Node<Set> * > tmp_nodes;

        Timer timer;
        while (!nodes.empty()
            && nodes.size() < balancer_params_.minimum_nodes) {
            node = nodes.top();
            nodes.pop();

            solver.branch(node, record, tmp_nodes, sol, initial_stats_);
            for (auto &set : tmp_nodes) {
                nodes.push(set);
            }
            tmp_nodes.clear();
        }
        initial_stats_.seconds = timer.elapsed_seconds();
    }

    // parallel part
    list_nodes_.resize(balancer_params_.threads);
    list_stats_.resize(balancer_params_.threads);
    for (unsigned i = 0; !nodes.empty(); ++i) {
        list_nodes_[i % balancer_params_.threads].push_back(nodes.top());
        nodes.pop();
    }

    std::vector<std::thread> threads(balancer_params_.threads);
    for (unsigned i = 0; i < balancer_params_.threads; ++i) {
        ++num_working_threads_;
        threads[i] = std::thread(&ParallelBNB::start, this, i);
    }

    std::for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

    list_nodes_.clear();

    sol.value = record_;
    sol.route.clear();
    return std::move(sol);
}

template <typename Solver, typename NodesContainer>
void ParallelBNB<Solver, NodesContainer>::start(unsigned threadID) {
    list_stats_[threadID].clear();
    value_type record = record_;
    Solution sol;
    auto nodes = make_nodes_container<Solver>(LifoContainer()
        , list_nodes_[threadID].begin(), list_nodes_[threadID].end());
    list_nodes_[threadID].clear();
    Node<Set> *node;
    Solver solver;
    solver.init(*initial_data_, &mm_);

    std::vector<Node<Set> * > tmp_nodes;

    Timer timer;
    while (!nodes.empty()) {
        node = nodes.top();
        nodes.pop();

        record = record_;
        solver.branch(node, record, tmp_nodes, sol, list_stats_[threadID]);
        for (auto &set : tmp_nodes) {
            nodes.push(set);
        }
        tmp_nodes.clear();

        if (record < record_) {
            std::lock_guard<std::mutex> lock(mutex_record_);
            if (record < record_) {
                record_ = record;
            }
        }

        if (nodes.size() > balancer_params_.maximum_nodes) {
            std::lock_guard<std::mutex> lock(mutex_sets_);
            while (nodes.size() > balancer_params_.minimum_nodes) {
                queue_sets_.push(nodes.top());
                nodes.pop();
                ++list_stats_[threadID].sets_sent;
            }
            condvar_sets_.notify_all();
        }

        if (nodes.empty()) {
            list_stats_[threadID].seconds += timer.elapsed_seconds();
            std::unique_lock<std::mutex> lock(mutex_sets_);
            --num_working_threads_;
            condvar_sets_.wait(lock,
                [this] {
                    return (!this->queue_sets_.empty())
                    || (this->num_working_threads_ == 0);
                });
            if (num_working_threads_ == 0) {
                condvar_sets_.notify_all();
                timer.reset();
                break;
            }
            while (nodes.size() < balancer_params_.minimum_nodes
                && !queue_sets_.empty()) {
                nodes.push(queue_sets_.front());
                queue_sets_.pop();
                ++list_stats_[threadID].sets_received;
            }
            ++num_working_threads_;
            timer.reset();
        }
    }
    list_stats_[threadID].seconds += timer.elapsed_seconds();
}

template <typename SolverFactory, typename NodesContainer>
void ParallelBNB<SolverFactory, NodesContainer>
::print_stats(std::ostream &os) const {
    os << std::endl;
    os << "Initial stats:\n" << initial_stats_ << std::endl;
    for (size_t i = 0; i < list_stats_.size(); ++i) {
        os << "Stats of thread #" << i << ":" << std::endl;
        os << list_stats_[i] << std::endl;
    }
}

#endif  // SRC_PARALLEL_BNB_INL_H_
