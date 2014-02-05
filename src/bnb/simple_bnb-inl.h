// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#ifndef BNB_SIMPLE_BNB_INL_H_
#define BNB_SIMPLE_BNB_INL_H_

namespace bnb {
    template <typename Solver>
    typename Solver::Solution
    SimpleBNB<Solver>::solve(
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

            std::stack<Node<Set> * > nodes;
            std::vector<Node<Set> * > tmp_nodes;
            typename std::vector<Node<Set> * >::iterator pos, end;
            nodes.push(node);
            UnixTimer timer;
            while (!nodes.empty() && !Signals::is_interrupted()) {
                node = nodes.top();
                nodes.pop();

                solver.branch(node, record, tmp_nodes, sol, stats_);

                for (pos = tmp_nodes.begin(), end = tmp_nodes.end()
                     ; pos != end
                     ; ++pos) {
                    nodes.push(*pos);
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
        }

        sol.value = record;
        sol.route.clear();
        return sol;
    }

    template <typename Solver>
    void SimpleBNB<Solver>::print_stats(std::ostream &os) const {
        os << "\nStats: \n" << stats_ << std::endl;
        os << "# of sets per second: ";
        os << (stats_.sets_generated / stats_.seconds) << std::endl;
    }

}  // namespace bnb
#endif  // BNB_SIMPLE_BNB_INL_H_
