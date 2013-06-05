// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_PARALLEL_BNB_INL_H_
#define SRC_PARALLEL_BNB_INL_H_

template <typename Solver, typename NodesContainer, typename Scheduler>
typename Solver::Solution
ParallelBNB<Solver, NodesContainer, Scheduler>::solve(
      const InitialData &data
    , size_t max_branches
    , value_type record) {
    static const size_t MIN_RANK_VALUE = 2;

    initial_data_ = &data;

    Solution sol;
    auto nodes = make_nodes_container<Solver>(LifoContainer());
    initial_stats_.clear();
    if (data.rank > MIN_RANK_VALUE) {
        Solver solver;
        solver.init(data, &search_tree_);

        Node<Set> *node = search_tree_.create_node();
        solver.get_initial_node(node);
        nodes.push(node);

        Solution initSol;
        solver.get_initial_solution(&initSol);
        record = initSol.value;

        std::vector<Node<Set> * > tmp_nodes;

        unsigned num_minimum_nodes =
            scheduler_.num_minimum_nodes() * scheduler_.num_threads();
        Timer timer;
        while (!nodes.empty()
            && nodes.size() < num_minimum_nodes) {
            node = nodes.top();
            nodes.pop();

            solver.branch(node, record, tmp_nodes, sol, initial_stats_);
            for (auto &set : tmp_nodes) {
                nodes.push(set);
            }
            tmp_nodes.clear();
            search_tree_.release_node(node);
        }
        record_ = record;
        initial_stats_.seconds = timer.elapsed_seconds();
    }

    // parallel part
    const unsigned num_threads = scheduler_.num_threads();
    list_nodes_.resize(num_threads);
    list_stats_.resize(num_threads);
    for (unsigned i = 0; !nodes.empty(); ++i) {
        list_nodes_[i % num_threads].push_back(nodes.top());
        nodes.pop();
    }

    std::vector<std::thread> threads(num_threads);
    for (unsigned i = 0; i < num_threads; ++i) {
        threads[i] = std::thread(&ParallelBNB::start, this, i);
    }

    std::for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

    list_nodes_.clear();

    sol.value = record_;
    sol.route.clear();
    return std::move(sol);
}

template <typename Solver, typename NodesContainer, typename Scheduler>
void ParallelBNB<Solver, NodesContainer, Scheduler>::start(unsigned threadID) {
    list_stats_[threadID].clear();
    value_type record = record_;
    Solution sol;
    auto nodes = make_nodes_container<Solver>(LifoContainer()
        , list_nodes_[threadID].begin(), list_nodes_[threadID].end());
    list_nodes_[threadID].clear();
    Node<Set> *node;
    Solver solver;
    solver.init(*initial_data_, &search_tree_);

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
        search_tree_.release_node(node);

        if (record < record_) {
            std::lock_guard<std::mutex> lock(mutex_record_);
            if (record < record_) {
                record_ = record;
            }
        }

        list_stats_[threadID].seconds += timer.elapsed_seconds();
        auto scheduler_stats = scheduler_.schedule(&nodes);
        timer.reset();
        list_stats_[threadID].sets_sent += scheduler_stats.sets_sent;
        list_stats_[threadID].sets_received += scheduler_stats.sets_received;
    }
    list_stats_[threadID].seconds += timer.elapsed_seconds();
}

template <typename SolverFactory, typename NodesContainer, typename Scheduler>
void ParallelBNB<SolverFactory, NodesContainer, Scheduler>
::print_stats(std::ostream &os) const {
    os << std::endl;
    os << "Initial stats:\n" << initial_stats_ << std::endl;
    Stats total_stats;
    for (size_t i = 0; i < list_stats_.size(); ++i) {
        os << "Stats of thread #" << i << ":" << std::endl;
        os << list_stats_[i] << std::endl;
        total_stats += list_stats_[i];
    }

    os << "Total stats:\n";
    os << total_stats << std::endl;

    Stats average_stats = total_stats / list_stats_.size();
    os << "Average stats:\n";
    os << average_stats << std::endl;

    os << "# of sets per second: ";
    os << (total_stats.sets_generated / average_stats.seconds) << std::endl;
}

#endif  // SRC_PARALLEL_BNB_INL_H_
