// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#ifndef BNB_TREE_INL_H_
#define BNB_TREE_INL_H_

#include <cassert>

#include <common/log.h>

namespace bnb {
template <typename D>
SearchTree<D>::SearchTree()
    : num_nodes_(0) {
}

template <typename D>
SearchTree<D>::~SearchTree() {
    CHECK(num_nodes_ == 0) << "SearchTree: unfreed memory: " << num_nodes_;
}

template <typename D>
Node<D> *SearchTree<D>::create_node(const Node<D> *parent) {
    ++num_nodes_;
    Node<D> *result = static_cast<Node<D> *>(allocator_.allocate());
    new(result)Node<D>;
    result->parent = parent;

    if (parent != nullptr) {
        allocator_.inc_refs(const_cast<Node<D> *>(parent));
    }

    assert(has_cycle(result) == false);
    return result;
}

template <typename D>
void SearchTree<D>::release_node(Node<D> *node) {
    while (node && allocator_.dec_refs(node) == 0) {
        node->~Node<D>();
        --num_nodes_;
        node->data.clear();
        node = const_cast<Node<D> *>(node->parent);
    }
}

template <typename D>
bool SearchTree<D>::has_cycle(const Node<D> *start) {
    const Node<D> *node = start->parent;
    while (node != nullptr) {
        if (node == start) {
            return true;
        }
        node = node->parent;
    }
    return false;
}
}  // namespace bnb
#endif  // BNB_TREE_INL_H_
