// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_TREE_INL_H_
#define SRC_TREE_INL_H_

#include <g2log.h>

template <typename D>
MemoryManager<D>::MemoryManager(size_t capacity)
    : refs_(0)
    , capacity_(capacity)
    , free_list_(nullptr) {
}

template <typename D>
MemoryManager<D>::~MemoryManager() {
    for (auto area : area_list_) {
        delete [] area;
    }
    CHECK(refs_ == 0) << "MemoryManager: unfreed memory: " << refs_;
}

template <typename D>
Node<D> *MemoryManager<D>::alloc(const Node<D> *parent) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (free_list_ == nullptr) {
        allocate();
    }

    ++refs_;
    Element *elem = free_list_;
    free_list_ = free_list_->header.next;
    elem->header.refs = 0;
    Node<D> *result = &(elem->data);
    inc_refs(result);
    result->parent = parent;

    if (parent != nullptr) {
        inc_refs(const_cast<Node<D> *>(parent));
    }

    CHECK(has_cycle(result) == false);
    return result;
}

template <typename D>
void MemoryManager<D>::free(Node<D> *ptr) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (dec_refs(ptr) == 0) {
        --refs_;
        ptr->data.clear();

        Element *elem = reinterpret_cast<Element *>(
            reinterpret_cast<int8_t *>(ptr) - sizeof(elem->header));
        elem->header.next = free_list_;
        free_list_ = elem;
        if (ptr->parent != nullptr) {
            free(const_cast<Node<D> *>(ptr->parent));
        }
    }
}

template <typename D>
bool MemoryManager<D>::has_cycle(const Node<D> *start) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    const Node<D> *node = start->parent;
    while (node != nullptr) {
        if (node == start) {
            return true;
        }
        node = node->parent;
    }
    return false;
}

template <typename D>
void MemoryManager<D>::inc_refs(Node<D> *node) {
    ++(reinterpret_cast<Element *>(
        reinterpret_cast<int8_t *>(node) - sizeof(size_t))->header.refs);
}

template <typename D>
size_t MemoryManager<D>::dec_refs(Node<D> *node) {
    return --(reinterpret_cast<Element *>(
        reinterpret_cast<int8_t *>(node) - sizeof(size_t))->header.refs);
}

template <typename D>
void MemoryManager<D>::allocate() {
    Element *area = new Element[capacity_];
    for (size_t i = 0; i < capacity_-1; ++i) {
        area[i].header.next = &area[i+1];
    }
    area[capacity_-1].header.next = free_list_;
    free_list_ = area;
    area_list_.push_back(area);
}

#endif  // SRC_TREE_INL_H_
