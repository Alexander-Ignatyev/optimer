// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#ifndef BNB_TREE_H_
#define BNB_TREE_H_

#include <common/allocator.h>

namespace bnb {
template <typename D>
struct Node {
    const Node<D> *parent;
    D data;
};

template <typename D>
class SearchTree {
 public:
    SearchTree();
    virtual ~SearchTree() {}

    virtual Node<D> *create_node(const Node<D> *parent = nullptr);
    virtual void release_node(Node<D> *node);

    // return this if no local thread search tree
    virtual SearchTree<D> *thread_local_tree() {
        return this;
    }

    virtual int num_unfreed_nodes() {
        return num_nodes_;
    }

 private:
    bool has_cycle(const Node<D> *start);

    int num_nodes_;
    RefCountedAllocator<sizeof(Node<D>)> allocator_;
};
}  // namespace bnb

#include "tree-inl.h"

#endif  // BNB_TREE_H_
