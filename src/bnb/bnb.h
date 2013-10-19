// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef BNB_BNB_H_
#define BNB_BNB_H_

#include <stack>
#include <queue>

#include "defs.h"
#include "tree.h"

namespace bnb {
struct LifoContainer { };

struct PriorityContainer { };

template<typename Solver>
std::stack<Node<typename Solver::Set> *>
    make_nodes_container(LifoContainer lifo) {
    return std::stack<Node<typename Solver::Set> *>();
}

template<typename Solver, typename InputIterator>
std::stack<Node<typename Solver::Set> *>
    make_nodes_container(LifoContainer lifo
    , InputIterator first, InputIterator last) {
    auto container = std::stack<Node<typename Solver::Set> *>();
    for (; first != last; ++first) {
        container.push(*first);
    }
    return std::move(container);
}

template<typename Set>
struct PtrNodeComparer {
    bool operator()(const Node<Set> *lhs, const Node<Set> *rhs) const {
        return lhs->data < rhs->data;
    }
};

template<typename Solver>
std::priority_queue<Node<typename Solver::Set> *
    , std::vector<Node<typename Solver::Set> *>
    , PtrNodeComparer<typename Solver::Set> >
    make_nodes_container(PriorityContainer priority) {
    return std::priority_queue<Node<typename Solver::Set> *
        , std::vector<Node<typename Solver::Set> *>
        , PtrNodeComparer<typename Solver::Set> >();
}

template<typename Solver, typename InputIterator>
std::priority_queue<Node<typename Solver::Set> *
    , std::vector<Node<typename Solver::Set> *>
    , PtrNodeComparer<typename Solver::Set> >
    make_nodes_container(PriorityContainer priority
        , InputIterator first, InputIterator last) {
    return std::priority_queue<Node<typename Solver::Set> *
        , std::vector<Node<typename Solver::Set> *>
        , PtrNodeComparer<typename Solver::Set> >()
        (first, last);
}
}  // namespace bnb
#endif  // BNB_BNB_H_
