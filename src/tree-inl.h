// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_TREE_INL_H_
#define SRC_TREE_INL_H_

template <typename D>
MemoryManager<D>::MemoryManager()
    : refs_(0)
    , inc_refs_(0)
    , capacity_(0)
    , area_(nullptr)
    , free_list_(nullptr) {
}

template <typename D>
MemoryManager<D>::~MemoryManager() {
    delete [] area_;
    assert(refs_ == 0);
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
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ++refs_;
    ++inc_refs_;
    if (free_list_ == NULL) {
        std::cerr << "MemoryManager: not enough memory\n";
        throw std::bad_alloc();
    }
    if (parent != NULL) {
        assert(CheckRefs(parent));
        IncRefs(const_cast<Node<D> *>(parent));
    }
    Element *elem = free_list_;
    free_list_ = free_list_->header.next;
    elem->header.refs = 0;

    Node<D> *result = &(elem->data);
    result->parent = parent;
    assert(CheckCycle(result));
    return result;
}

template <typename D>
void MemoryManager<D>::free(Node<D> *ptr) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    --refs_;
    ptr->data.ap_solve.clear();
    Element *elem = reinterpret_cast<Element *>(
        reinterpret_cast<int8_t *>(ptr) - sizeof(elem->header));
    assert(elem->header.refs == 0);
    elem->header.next = free_list_;
    free_list_ = elem;
    if (elem->data.parent != NULL
        && !DecRefs(const_cast<Node<D> *>(elem->data.parent))) {
        assert(ptr->parent == elem->data.parent);
        ptr = const_cast<Node<D> *>(elem->data.parent);
        this->free(ptr);
    }
}

template <typename D>
bool MemoryManager<D>::CheckCycle(const Node<D> *start) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    const Node<D> *node = start->parent;
    while (node != NULL) {
        if (node == start) {
            return false;
        }
        node = node->parent;
    }
    return true;
}

template <typename D>
bool MemoryManager<D>::CheckRefs(const Node<D> *node) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    const Element * elem = reinterpret_cast<const Element *>(
        reinterpret_cast<const int8_t *>(node) - sizeof(size_t));
    return elem->header.refs <= 2;
}

template <typename D>
void MemoryManager<D>::IncRefs(Node<D> *node) {
#ifndef NDEBUG
    Element * elem = reinterpret_cast<Element *>(
        reinterpret_cast<int8_t *>(node) - sizeof(size_t));
    (elem->header.refs)++;
    assert(elem >= area_ && elem <= &area_[capacity_]);
    assert(elem->header.refs == 1 || elem->header.refs == 2);
#else
    ++(reinterpret_cast<Element *>(
        reinterpret_cast<int8_t *>(node) - sizeof(size_t))->header.refs);
#endif
}

template <typename D>
size_t MemoryManager<D>::DecRefs(Node<D> *node) {
#ifndef NDEBUG
    Element * elem = reinterpret_cast<Element *>(
        reinterpret_cast<int8_t *>(node) - sizeof(size_t));
    assert(elem->header.refs == 1 || elem->header.refs == 2);
    return --(elem->header.refs);
#else
    return --(reinterpret_cast<Element *>(
        reinterpret_cast<int8_t *>(node) - sizeof(size_t))->header.refs);
#endif
}

#endif  // SRC_TREE_INL_H_
