// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_AP_SOLVER_H_
#define SRC_AP_SOLVER_H_

#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>

template <typename T>
inline bool is_zero(T value) {
    return value == 0;
}

template <>
inline bool is_zero<double> (double value) {
    return std::abs(value) < std::numeric_limits<double>::epsilon();
}

template <>
inline bool is_zero<float> (float value) {
    return std::abs(value) < std::numeric_limits<float>::epsilon();
}

template <typename T>
class ApSolver {
 public:
    ApSolver(): dimension_(0), matrix_(nullptr) {}
    const std::vector<size_t> &solve(const T *matrix, size_t dimension);

 private:
    void preprocessing();
    size_t augment(size_t k);
    void dual_update(const std::vector<char> &scanned_u
                    , std::vector<char> *plabeled_v
                    , std::vector<T> *ppi);
    size_t next_unassigned() const;

    static const T big_value_;
    size_t dimension_;
    const T *matrix_;
    std::vector<T> dual_u_;
    std::vector<T> dual_v_;
    std::vector<size_t> row_;
    std::vector<size_t> phi_;
    std::vector<size_t> pred_;
};

template <typename T>
const std::vector<size_t> &ApSolver<T>::solve(const T *matrix
                                            , size_t dimension) {
    matrix_ = matrix;
    dimension_ = dimension;

    preprocessing();

    size_t k = next_unassigned();
    while (k != dimension_) {
        size_t j = augment(k);
        size_t i;
        do {
            i = pred_[j];
            row_[j] = i;
            size_t h = j;
            j = phi_[i];
            phi_[i] = h;
        } while (i != k);
        k = next_unassigned();
    }

    return phi_;
}

template <typename T>
void ApSolver<T>::preprocessing() {
    // dual problem: row and column reduction
    dual_u_.resize(dimension_);
    dual_v_.resize(dimension_);
    std::fill(dual_u_.begin(), dual_u_.end(), big_value_);
    std::fill(dual_v_.begin(), dual_v_.end(), big_value_);

    for (size_t i = 0; i < dimension_; ++i) {
        for (size_t j = 0; j < dimension_; ++j) {
            dual_u_[i] = std::min(dual_u_[i], matrix_[i*dimension_+j]);
        }
    }

    for (size_t i = 0; i < dimension_; ++i) {
        for (size_t j = 0; j < dimension_; ++j) {
            dual_v_[j] = std::min(dual_v_[j]
                        , matrix_[i*dimension_+j]-dual_u_[i]);
        }
    }

    // primal problem: partial solution
    row_.resize(dimension_);
    phi_.resize(dimension_, dimension_);
    std::fill(row_.begin(), row_.end(), dimension_);
    std::fill(phi_.begin(), phi_.end(), dimension_);

    for (size_t i = 0; i < dimension_; ++i) {
        for (size_t j = 0; j < dimension_; ++j) {
            if (row_[j] == dimension_
                && matrix_[i*dimension_+j] == dual_u_[i]+dual_v_[j]) {
                row_[j] = i;
                break;
            }
        }
    }

    for (size_t j = 0; j < dimension_; ++j) {
        if (row_[j] < dimension_) {
            phi_[row_[j]] = j;
        }
    }
}

template <typename T>
size_t ApSolver<T>::augment(size_t k) {
    std::vector<T> pi(dimension_, big_value_);
    std::vector<char> scanned_u(dimension_, 0);
    std::vector<char> scanned_v(dimension_, 0);
    std::vector<char> labeled_v(dimension_, 0);
    pred_.resize(dimension_);
    std::fill(pred_.begin(), pred_.end(), dimension_);

    size_t sink = dimension_;
    size_t i = k;
    while (sink == dimension_) {
        scanned_u[i] = 1;
        for (size_t j = 0; j < dimension_; ++j) {
            T val = matrix_[i*dimension_+j] - dual_u_[i] - dual_v_[j];
            if (labeled_v[j] == 0 && pi[j] > val) {
                pred_[j] = i;
                pi[j] = val;
                if (is_zero(val)) {
                    labeled_v[j] = 1;
                }
            }
        }
        bool update_labeled = false;
        for (size_t j = 0; j < dimension_; ++j) {
            if (labeled_v[j] == 1 && scanned_v[j] == 0) {
                scanned_v[j] = 1;
                update_labeled = true;
                if (row_[j] == dimension_) {
                    sink = j;
                } else {
                    i = row_[j];
                }
            }
        }

        if (!update_labeled) {
            dual_update(scanned_u, &labeled_v, &pi);
        }
    }
    return sink;
}

template <typename T>
void ApSolver<T>::dual_update(const std::vector<char> &scanned_u
                              , std::vector<char> *plabeled_v
                              , std::vector<T> *ppi) {
    std::vector<T> &pi = *ppi;
    std::vector<char> &labeled_v = *plabeled_v;

    T delta = big_value_;
    for (size_t j = 0; j < dimension_; ++j) {
        if (labeled_v[j] == 0) {
            delta = std::min(delta, pi[j]);
        }
    }
    for (size_t k = 0; k < dimension_; ++k) {
        if (scanned_u[k] == 1) {
            dual_u_[k] += delta;
        }
        if (labeled_v[k] == 1) {
            dual_v_[k] -= delta;
        } else {  // if (labeled_v[k] == 0)
            pi[k] -= delta;
            if (is_zero(pi[k])) {
                labeled_v[k] = 1;
            }
        }
    }
}

template <typename T>
size_t ApSolver<T>::next_unassigned() const {
    for (size_t k = 0; k < dimension_; ++k) {
        if (phi_[k] == dimension_) {
            return k;
        }
    }
    return dimension_;
}

template <typename T>
const T ApSolver<T>::big_value_ = std::numeric_limits<T>::max();

#endif  // SRC_AP_SOLVER_H_
