// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_TREE_INL_H_
#define SRC_TREE_INL_H_

#include <g2log.h>

template <typename D>
MemoryManager<D>::MemoryManager()
    : refs_(0)
    , capacity_(0)
    , area_(nullptr)
    , free_list_(nullptr) {
}

template <typename D>
MemoryManager<D>::~MemoryManager() {
    delete [] area_;
    CHECK(refs_ == 0) << "MemoryManager: unfreed memory: " << refs_;
}

template <typename D>
void MemoryManager<D>::init(size_t capacity) {
    if (capacity_ < capacity) {
        capacity_ = capacity;
        delete [] area_;
        area_ = new Element[capacity];
        for (size_t i = 0; i < capacity-1; ++i) {
            area_[i].header.next = &area_[i+1];
        }
        area_[capacity-1].header.next = NULL;
        free_list_ = area_;
    }
}

template <typename D>
Node<D> *MemoryManager<D>::alloc(const Node<D> *parent) {
    CHECK(free_list_ != nullptr) << "MemoryManager: not enough memory";

    std::lock_guard<std::recursive_mutex> lock(mutex_);

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
    ptr->data.ap_solve.clear();
    if (dec_refs(ptr) == 0) {
        --refs_;
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

#endif  // SRC_TREE_INL_H_
