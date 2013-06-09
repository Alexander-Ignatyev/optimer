// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_ALLOCATOR_INL_H_
#define COMMON_ALLOCATOR_INL_H_

#include <cstdint>

template <size_t N>
struct RefCountedAllocator<N>::Element {
    union {
        Element *next;
        size_t refs;
    } header;
    char data[N];
};

template <size_t N>
RefCountedAllocator<N>::RefCountedAllocator(size_t capacity)
    : capacity_(capacity)
    , free_list_(nullptr) {
}

template <size_t N>
RefCountedAllocator<N>::~RefCountedAllocator() {
    for (auto area : area_list_) {
        delete [] area;
    }
}

template <size_t N>
void *RefCountedAllocator<N>::allocate() {
    if (free_list_ == nullptr) {
        reserve();
    }

    Element *elem = free_list_;
    free_list_ = free_list_->header.next;
    elem->header.refs = 0;
    inc_refs(elem->data);
    return elem->data;
}

template <size_t N>
size_t RefCountedAllocator<N>::inc_refs(void *data) {
    return ++(element(data)->header.refs);
}

template <size_t N>
size_t RefCountedAllocator<N>::dec_refs(void *data) {
    size_t refs = --(element(data)->header.refs);
    if (refs == 0) {
        Element *elem = element(data);
        elem->header.next = free_list_;
        free_list_ = elem;
    }
    return refs;
}

template <size_t N>
void RefCountedAllocator<N>::reserve() {
    Element *area = new Element[capacity_];
    for (size_t i = 0; i < capacity_-1; ++i) {
        area[i].header.next = &area[i+1];
    }
    area[capacity_-1].header.next = free_list_;
    free_list_ = area;
    area_list_.push_back(area);
}

template <size_t N>
typename RefCountedAllocator<N>::Element *
    RefCountedAllocator<N>::element(void *data) {
    return reinterpret_cast<Element *>(
        reinterpret_cast<int8_t *>(data) - sizeof(Element::header));
}

#include "allocator-inl.h"

#endif  // COMMON_ALLOCATOR_INL_H_
