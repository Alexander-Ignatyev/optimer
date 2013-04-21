// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_SEQUENCE_BNB_INL_H_
#define SRC_SEQUENCE_BNB_INL_H_

template <typename Solver, typename NodesContainer>
typename Solver::Solution
    SequenceBNB<Solver, NodesContainer>::solve(
    const InitialData &data, size_t max_branches, value_type record) {
    static const size_t MIN_RANK_VALUE = 2;

    stats_.clear();
    Solution sol;
    if (data.rank > MIN_RANK_VALUE) {
        MemoryManager<Set> mm;
        mm.init(data.rank*data.rank*data.rank*data.rank);
        Node<Set> *node = mm.alloc(NULL);
        Solution initSol;

        Solver solver;
        solver.init(data, &mm);
        solver.get_initial_node(node);
        solver.get_initial_solution(&initSol);
        record = initSol.value;

        auto nodes = make_nodes_container<Solver>(NodesContainer());
        std::vector<Node<Set> * > tmp_nodes;
        nodes.push(node);
        Timer timer;
        while (!nodes.empty() /*&& this->stats.branches < max_branches*/) {
            node = nodes.top();
            nodes.pop();

            solver.branch(node, record, tmp_nodes, sol, stats_);
            for (auto &set : tmp_nodes) {
                nodes.push(set);
            }
            tmp_nodes.clear();
        }
        stats_.seconds = timer.elapsed_seconds();
    }

    sol.value = record;
    sol.route.clear();
    return std::move(sol);
}

#endif  // SRC_SEQUENCE_BNB_INL_H_
