// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef TSP_ASYMMETRIC_AP_SOLVER_H_
#define TSP_ASYMMETRIC_AP_SOLVER_H_

#include <cmath>

#include <vector>
#include <limits>
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
     struct Solution {
         std::vector<T> dual_u;
         std::vector<T> dual_v;
         std::vector<size_t> primal;

         void init(size_t dimension) {
             clear();
             dual_u.resize(dimension);
             dual_v.resize(dimension);
             primal.resize(dimension);
         }
         bool empty() const {
             return primal.empty();
         }

         void clear() {
             dual_u.clear();
             dual_v.clear();
             primal.clear();
         }
     };

    ApSolver(): dimension_(0), matrix_(nullptr) {}
    void solve(const T *matrix, size_t dimension, Solution *solution);

 private:
    void preprocess(Solution *solution);
    size_t augment(size_t k, Solution *solution);
    void dual_update(const std::vector<char> &scanned_u
                    , std::vector<char> *plabeled_v
                    , std::vector<T> *ppi
                    , Solution *solution) const;
    size_t next_unassigned(const Solution *solution) const;

    static const T big_value_;

    size_t dimension_;
    const T *matrix_;
    std::vector<size_t> row_;
    std::vector<size_t> pred_;
};

template <typename T>
void ApSolver<T>::solve(const T *matrix
                            , size_t dimension
                            , Solution *solution) {
    matrix_ = matrix;
    dimension_ = dimension;

    preprocess(solution);

    size_t *phi = &solution->primal[0];
    size_t k = next_unassigned(solution);
    while (k != dimension_) {
        size_t j = augment(k, solution);
        size_t i;
        do {
            i = pred_[j];
            row_[j] = i;
            size_t h = j;
            j = phi[i];
            phi[i] = h;
        } while (i != k);
        k = next_unassigned(solution);
    }
}

template <typename T>
void ApSolver<T>::preprocess(Solution *solution) {
    if (solution->empty()) {
        // dual problem: row and column reduction
        solution->dual_u.resize(dimension_, big_value_);
        solution->dual_v.resize(dimension_, big_value_);
        T *dual_u = &solution->dual_u[0];
        T *dual_v = &solution->dual_v[0];

        for (size_t i = 0; i < dimension_; ++i) {
            for (size_t j = 0; j < dimension_; ++j) {
                dual_u[i] = std::min(dual_u[i], matrix_[i*dimension_+j]);
            }
        }

        for (size_t i = 0; i < dimension_; ++i) {
            for (size_t j = 0; j < dimension_; ++j) {
                dual_v[j] = std::min(dual_v[j]
                            , matrix_[i*dimension_+j]-dual_u[i]);
            }
        }

        // primal problem: partial solution
        row_.resize(dimension_);
        std::fill(row_.begin(), row_.end(), dimension_);
        solution->primal.resize(dimension_, dimension_);
        size_t *phi = &solution->primal[0];

        for (size_t i = 0; i < dimension_; ++i) {
            for (size_t j = 0; j < dimension_; ++j) {
                if (row_[j] == dimension_
                    && matrix_[i*dimension_+j] == dual_u[i]+dual_v[j]) {
                    row_[j] = i;
                    break;
                }
            }
        }

        for (size_t j = 0; j < dimension_; ++j) {
            if (row_[j] < dimension_) {
                phi[row_[j]] = j;
            }
        }
    } else {
        row_.resize(dimension_);
        std::fill(row_.begin(), row_.end(), dimension_);
        size_t *phi = &solution->primal[0];

        for (size_t i = 0; i < dimension_; ++i) {
            if (phi[i] < dimension_) {
                row_[phi[i]] = i;
            }
        }
    }
}

template <typename T>
size_t ApSolver<T>::augment(size_t k, Solution *solution) {
    std::vector<T> pi(dimension_, big_value_);
    std::vector<char> scanned_u(dimension_, 0);
    std::vector<char> scanned_v(dimension_, 0);
    std::vector<char> labeled_v(dimension_, 0);
    pred_.resize(dimension_);
    std::fill(pred_.begin(), pred_.end(), dimension_);
    T *dual_u = &solution->dual_u[0];
    T *dual_v = &solution->dual_v[0];

    size_t sink = dimension_;
    size_t i = k;
    while (sink == dimension_) {
        scanned_u[i] = 1;
        const T  *matrix_i = &matrix_[i*dimension_];
        for (size_t j = 0; j < dimension_; ++j) {
            T val = matrix_i[j] - dual_u[i] - dual_v[j];
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
                break;
            }
        }

        if (!update_labeled) {
            dual_update(scanned_u, &labeled_v, &pi, solution);
        }
    }
    return sink;
}

template <typename T>
void ApSolver<T>::dual_update(const std::vector<char> &scanned_u
                              , std::vector<char> *plabeled_v
                              , std::vector<T> *ppi
                              , Solution *solution) const {
    std::vector<T> &pi = *ppi;
    std::vector<char> &labeled_v = *plabeled_v;
    T *dual_u = &solution->dual_u[0];
    T *dual_v = &solution->dual_v[0];

    T delta = big_value_;
    for (size_t j = 0; j < dimension_; ++j) {
        if (labeled_v[j] == 0) {
            delta = std::min(delta, pi[j]);
        }
    }
    for (size_t k = 0; k < dimension_; ++k) {
        if (scanned_u[k] == 1) {
            dual_u[k] += delta;
        }
        if (labeled_v[k] == 1) {
            dual_v[k] -= delta;
        } else {  // if (labeled_v[k] == 0)
            pi[k] -= delta;
            if (is_zero(pi[k])) {
                labeled_v[k] = 1;
            }
        }
    }
}

template <typename T>
size_t ApSolver<T>::next_unassigned(const Solution *solution) const {
    const size_t *phi = &solution->primal[0];
    for (size_t k = 0; k < dimension_; ++k) {
        if (phi[k] >= dimension_) {
            return k;
        }
    }
    return dimension_;
}

template <typename T>
const T ApSolver<T>::big_value_ = std::numeric_limits<T>::max();

#endif  // TSP_ASYMMETRIC_AP_SOLVER_H_
