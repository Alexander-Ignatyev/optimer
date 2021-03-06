// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#ifndef BNB_PARALLEL_BNB_INL_H_
#define BNB_PARALLEL_BNB_INL_H_

#include <common/stats.h>

namespace bnb {
template <typename Container, typename Set>
void clean_nodes(Container &nodes, SearchTree<Set> &search_tree) {
    while (!nodes.empty()) {
        Node<Set> *node = nodes.top();
        nodes.pop();
        search_tree.release_node(node);
    }
}

template <typename Solver, typename NodesContainer, typename Scheduler>
typename Solver::Solution
ParallelBNB<Solver, NodesContainer, Scheduler>::solve(
      const InitialData &data
    , size_t max_branches
    , value_type record) {
    static const size_t MIN_RANK_VALUE = 2;

    Signals::InterruptingSignalGuard signal_guard;

    initial_data_ = &data;

    SearchTree<Set> *search_tree  = search_tree_->thread_local_tree();
    Solution sol;
    auto nodes = make_nodes_container<Solver>(NodesContainer());
    initial_stats_.clear();
    if (data.rank > MIN_RANK_VALUE) {
        Solver solver;
        solver.init(data, search_tree);

        Node<Set> *node = search_tree->create_node();
        solver.get_initial_node(node);
        nodes.push(node);

        Solution initSol;
        solver.get_initial_solution(&initSol);
        if (record > initSol.value) {
            record = initSol.value;
        }

        std::vector<Node<Set> * > tmp_nodes;

        unsigned num_minimum_nodes =
            scheduler_.num_minimum_nodes() * scheduler_.num_threads();
        Timer timer;
        while (!nodes.empty()
            && nodes.size() < num_minimum_nodes
            && !Signals::is_interrupted()) {
            node = nodes.top();
            nodes.pop();

            solver.branch(node, record, tmp_nodes, sol, initial_stats_);
            for (auto &set : tmp_nodes) {
                nodes.push(set);
            }
            tmp_nodes.clear();
            search_tree->release_node(node);
        }

        record_ = record;
        initial_stats_.seconds = timer.elapsed_seconds();
    }

    if (!Signals::is_interrupted()) {
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
    }

    clean_nodes(nodes, *search_tree);

    list_nodes_.clear();

    sol.value = record_;
    sol.route.clear();

    CHECK(search_tree_->num_unfreed_nodes() == 0) <<
        "SearchTree: unfreed memory: " << search_tree_->num_unfreed_nodes();
    return std::move(sol);
}

template <typename Solver, typename NodesContainer, typename Scheduler>
void ParallelBNB<Solver, NodesContainer, Scheduler>::start(unsigned threadID) {
    list_stats_[threadID].clear();
    value_type record = record_;
    SearchTree<Set> *search_tree = search_tree_->thread_local_tree();
    Solution sol;
    auto nodes = make_nodes_container<Solver>(LifoContainer()
        , list_nodes_[threadID].begin(), list_nodes_[threadID].end());
    list_nodes_[threadID].clear();
    Node<Set> *node;
    Solver solver;
    solver.init(*initial_data_, search_tree);

    std::vector<Node<Set> * > tmp_nodes;

    Timer timer;
    while (!nodes.empty() && !Signals::is_interrupted()) {
        node = nodes.top();
        nodes.pop();

        record = record_;
        solver.branch(node, record, tmp_nodes, sol, list_stats_[threadID]);
        for (auto &set : tmp_nodes) {
            nodes.push(set);
        }
        tmp_nodes.clear();
        search_tree->release_node(node);

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

    clean_nodes(nodes, *search_tree);

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

    std::vector<double> times;
    for (size_t i = 0; i < list_stats_.size(); ++i) {
        times.push_back(list_stats_[i].seconds);
    }
    double seconds_mean = ::Stats::mean(&times[0], times.size());
    double seconds_variance = ::Stats::variance(&times[0], times.size());
    double seconds_max = ::Stats::max(&times[0], times.size());
    double seconds_min = ::Stats::min(&times[0], times.size());

    os << "Time mean in seconds: " << seconds_mean << std::endl;
    os << "Time variance: " << seconds_variance << std::endl;

    os << "Time max / min / difference: ";
    os << seconds_max << " / " << seconds_min;
    os << " / " << (seconds_max-seconds_min) << std::endl;

    os << "# of sets per mean time: ";
    os << (total_stats.sets_generated / seconds_mean) << std::endl;

    os << "# of sets per max time: ";
    os << (total_stats.sets_generated / seconds_max) << std::endl;
}

}  // namespace bnb

#endif  // BNB_PARALLEL_BNB_INL_H_
