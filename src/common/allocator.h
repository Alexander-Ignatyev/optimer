// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_ALLOCATOR_H_
#define COMMON_ALLOCATOR_H_

#include <list>
#include <common/nullptr.h>

template <size_t N>
class RefCountedAllocator {
 public:
    explicit RefCountedAllocator(size_t capacity = 1024);
    ~RefCountedAllocator();

    void *allocate();
    static size_t inc_refs(void *data);
    size_t dec_refs(void *data);

 private:
    RefCountedAllocator(const RefCountedAllocator &);
    RefCountedAllocator &operator=(const RefCountedAllocator &);

    struct Element;
    void reserve();
    static Element *element(void *data);

    size_t capacity_;
    std::list<Element *> area_list_;
    Element *free_list_;
};

#include "allocator-inl.h"

#endif  // COMMON_ALLOCATOR_H_
