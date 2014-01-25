// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#ifndef BNB_TREE_H_
#define BNB_TREE_H_

#ifndef SINGLE_THREADED
#include <mutex>
#endif

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
    ~SearchTree();

    Node<D> *create_node(const Node<D> *parent = nullptr);
    void release_node(Node<D> *node);

 private:
    bool has_cycle(const Node<D> *start);

    size_t num_nodes_;
    RefCountedAllocator<sizeof(Node<D>)> allocator_;
#ifndef SINGLE_THREADED
    std::mutex mutex_;
#endif
};
}  // namespace bnb

#include "tree-inl.h"

#endif  // BNB_TREE_H_
