// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_SERIAL_BNB_INL_H_
#define SRC_SERIAL_BNB_INL_H_

template <typename Solver, typename NodesContainer>
typename Solver::Solution
    SerialBNB<Solver, NodesContainer>::solve(
    const InitialData &data, size_t max_branches, value_type record) {
    static const size_t MIN_RANK_VALUE = 2;

    stats_.clear();
    Solution sol;
    if (data.rank > MIN_RANK_VALUE) {
        BnbSearchTree<Set> search_tree;
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
        while (!nodes.empty()) {
            node = nodes.top();
            nodes.pop();

            solver.branch(node, record, tmp_nodes, sol, stats_);
            for (auto &set : tmp_nodes) {
                nodes.push(set);
            }
            tmp_nodes.clear();
            search_tree.release_node(node);
        }
        stats_.seconds = timer.elapsed_seconds();
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

#endif  // SRC_SERIAL_BNB_INL_H_
