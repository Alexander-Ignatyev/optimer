// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#ifndef BNB_LOCKED_TREE_H_
#define BNB_LOCKED_TREE_H_

#include <mutex>

#include <common/allocator.h>

#include "tree.h"

namespace bnb {
template <typename D>
class LockedSearchTree: public SearchTree<D> {
 public:
    Node<D> *create_node(const Node<D> *parent = nullptr) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return SearchTree<D>::create_node(parent);
    }

    void release_node(Node<D> *node) override {
        std::lock_guard<std::mutex> lock(mutex_);
        SearchTree<D>::release_node(node);
    }
 private:
    std::mutex mutex_;
};
}  // namespace bnb

#include "tree-inl.h"

#endif  // BNB_LOCKED_TREE_H_
