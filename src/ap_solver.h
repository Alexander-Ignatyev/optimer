// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_AP_SOLVER_H_
#define SRC_AP_SOLVER_H_

#include <cstddef>

#include <memory>
#include <vector>
#include <limits>
#include <algorithm>

template <typename T>
class APSolver {
 public:
    APSolver();
    std::vector<size_t> transform(const T *data, size_t dimension);

 private:
    APSolver(const APSolver &) = delete;
    APSolver &operator=(const APSolver &) = delete;

    void alloc(size_t dimension);

    size_t dimension_;
    T *u_;
    T *v_;
    T *mins_;
    ptrdiff_t *links_;
    ptrdiff_t *visited_;
    size_t *mark_indices_;

    std::vector<ptrdiff_t> int_buffer_;
    std::vector<size_t> size_t_buffer_;
    std::vector<T> value_type_buffer_;
};

template <typename T>
APSolver<T>::APSolver()
        : dimension_(0)
        , u_(nullptr)
        , v_(nullptr)
        , mins_(nullptr)
        , links_(nullptr)
        , visited_(nullptr) {
    }

template <typename T>
void APSolver<T>::alloc(size_t dimension) {
    if (dimension_ == dimension || dimension == 0) {
        return;
    }
    dimension_ = dimension;
    int_buffer_.resize(dimension*2);
    links_ = &int_buffer_[0];
    visited_ = &int_buffer_[dimension];

    size_t_buffer_.resize(dimension);
    mark_indices_ = &size_t_buffer_[0];

    value_type_buffer_.resize(dimension*3);
    mins_ = &value_type_buffer_[0];
    u_ = &value_type_buffer_[dimension];
    v_ = &value_type_buffer_[dimension*2];
}

template <typename T>
std::vector<size_t> APSolver<T>::transform(const T *data, size_t dimension) {
    alloc(dimension);
    std::fill_n<T *>(u_, dimension, 0);
    std::fill_n<T *>(v_, dimension, 0);
    std::fill_n<size_t *>(mark_indices_, dimension, -1);

    for (size_t i = 0; i < dimension; i++) {
        std::fill_n<ptrdiff_t *>(links_, dimension, -1);
        std::fill_n<T *>(mins_, dimension, std::numeric_limits<T>::max());
        std::fill_n<ptrdiff_t *>(visited_, dimension, 0);

        ptrdiff_t markedI = i, markedJ = -1, j;
        while (markedI != -1) {
            j = -1;
            for (size_t j1 = 0; j1 < dimension; ++j1)
            if (!visited_[j1]) {
                auto value = data[markedI*dimension+j1] - u_[markedI] - v_[j1];
                if (mins_[j1] > value) {
                    mins_[j1] = value;
                    links_[j1] = markedJ;
                }
                if (j == -1 || mins_[j1] < mins_[j]) {
                    j = j1;
                }
            }

            T delta = mins_[j];
            for (size_t j1 = 0; j1 < dimension; j1++) {
                if (visited_[j1]) {
                    u_[mark_indices_[j1]] += delta;
                    v_[j1] -= delta;
                } else {
                    mins_[j1] -= delta;
                }
            }
            u_[i] += delta;

            visited_[j] = 1;
            markedJ = j;
            markedI = mark_indices_[j];
        }

        for (; links_[j] != -1; j = links_[j]) {
            mark_indices_[j] = mark_indices_[links_[j]];
        }
        mark_indices_[j] = i;
    }

    return std::vector<size_t>(mark_indices_, mark_indices_+dimension);
}

#endif  // SRC_AP_SOLVER_H_
