// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#ifndef BNB_SERIAL_BNB_INL_H_
#define BNB_SERIAL_BNB_INL_H_

namespace bnb {
template <typename Solver, typename NodesContainer>
typename Solver::Solution
    SerialBNB<Solver, NodesContainer>::solve(
    const InitialData &data, size_t max_branches, value_type record) {

    Signals::InterruptingSignalGuard signal_guard;

    static const size_t MIN_RANK_VALUE = 2;

    stats_.clear();
    Solution sol;
    if (data.rank > MIN_RANK_VALUE) {
        SearchTree<Set> search_tree;
        Node<Set> *node = search_tree.create_node();
        Solution initSol;

        Solver solver;
        solver.init(data, &search_tree);
        solver.get_initial_node(node);
        solver.get_initial_solution(&initSol);
        record = initSol.value;

        auto nodes = make_nodes_container<Solver>(NodesContainer());
        std::vector<Node<Set> * > tmp_nodes;
        nodes.push(node);
        Timer timer;
        while (!nodes.empty() && !Signals::is_interrupted()) {
            node = nodes.top();
            nodes.pop();

            solver.branch(node, record, tmp_nodes, sol, stats_);
            for (auto &set : tmp_nodes) {
                nodes.push(set);
            }
            tmp_nodes.clear();
            search_tree.release_node(node);
        }

        // cleaning in case of intterrupting
        while (!nodes.empty()) {
            node = nodes.top();
            nodes.pop();
            search_tree.release_node(node);
        }
        stats_.seconds = timer.elapsed_seconds();

        CHECK(search_tree.num_unfreed_nodes() == 0) <<
            "SearchTree: unfreed memory: " << search_tree.num_unfreed_nodes();
    }

    sol.value = record;
    sol.route.clear();
    return std::move(sol);
}

template <typename Solver, typename NodesContainer>
void SerialBNB<Solver, NodesContainer>::print_stats(std::ostream &os) const {
    os << "\nStats: \n" << stats_ << std::endl;
    os << "# of sets per second: ";
    os << (stats_.sets_generated / stats_.seconds) << std::endl;
}

}  // namespace bnb
#endif  // BNB_SERIAL_BNB_INL_H_
