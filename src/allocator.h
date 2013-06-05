// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_ALLOCATOR_H_
#define SRC_ALLOCATOR_H_

#include <list>

template <size_t N>
class RefCountedAllocator {
 public:
    explicit RefCountedAllocator(size_t capacity = 1024);
    ~RefCountedAllocator();

    RefCountedAllocator(const RefCountedAllocator &) = delete;
    RefCountedAllocator &operator=(const RefCountedAllocator &) = delete;

    void *allocate();
    static size_t inc_refs(void *data);
    size_t dec_refs(void *data);

 private:
    struct Element;
    void reserve();
    static Element *element(void *data);

    size_t capacity_;
    std::list<Element *> area_list_;
    Element *free_list_;
};

#include "allocator-inl.h"

#endif  // SRC_ALLOCATOR_H_
