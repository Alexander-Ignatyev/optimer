// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#ifndef BNB_MT_TREE_H_
#define BNB_MT_TREE_H_

#include <mutex>
#include <thread>
#include <memory>
#include <unordered_map>

#include <common/allocator.h>

#include "tree.h"

namespace bnb {
template <typename D>
class MTSearchTree: public SearchTree<D> {
 public:
    Node<D> *create_node(const Node<D> *parent = nullptr) override {
        return thread_local_tree()->create_node(parent);
    }

    void release_node(Node<D> *node) override {
        thread_local_tree()->release_node(node);
    }

    SearchTree<D> *thread_local_tree() override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto pos = map_.find(std::this_thread::get_id());
        if (pos != map_.end()) {
            return pos->second.get();
        } else {
            SearchTree<D> *tree = new SearchTree<D>();
            map_[std::this_thread::get_id()].reset(tree);
            return tree;
        }
    }

    int num_unfreed_nodes() override {
        int num_nodes = 0;
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto pos = map_.begin(); pos != map_.end(); ++pos) {
            num_nodes += pos->second->num_unfreed_nodes();
        }
        return num_nodes;
    }

 private:
    std::mutex mutex_;
    std::unordered_map<std::thread::id, std::unique_ptr<SearchTree<D> > > map_;
};
}  // namespace bnb

#include "tree-inl.h"

#endif  // BNB_MT_TREE_H_
