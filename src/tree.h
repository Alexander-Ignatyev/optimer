// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_TREE_H_
#define SRC_TREE_H_

#include <mutex>

#include "allocator.h"

template <typename D>
struct Node {
    const Node<D> *parent;
    D data;
};

template <typename D>
class BnbSearchTree {
 public:
    BnbSearchTree();
    ~BnbSearchTree();

    Node<D> *create_node(const Node<D> *parent = nullptr);
    void release_node(Node<D> *node);

 private:
    bool has_cycle(const Node<D> *start);

    size_t num_nodes_;
    RefCountedAllocator<sizeof(Node<D>)> allocator_;
    std::mutex mutex_;
};

#include "tree-inl.h"

#endif  // SRC_TREE_H_
