// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#ifndef TSP_SYMMETRIC_MS_ONE_TREE_INL_H_
#define TSP_SYMMETRIC_MS_ONE_TREE_INL_H_

namespace MSOneTree {
template <typename T>
const T max_val() {
    return std::numeric_limits<T>::max() / 2;
}

namespace Prim {
bool vertex_is_included(size_t vertex, const std::vector<size_t> &min_indices);

template <typename T>
void update_mins(const T *matrix
                 , size_t size
                 , size_t index
                 , std::vector<size_t> *pmin_indices
                 , std::vector<T> *pmin_values) {
    assert(pmin_indices);
    assert(pmin_values);

    std::vector<size_t> &min_indices = *pmin_indices;
    std::vector<T> &min_values = *pmin_values;

    for (size_t i = 1; i < size; ++i) {
        if (min_indices[i] == index) {
            min_values[i] = max_val<T>();
            for (size_t j = 1; j < size; ++j) {
                if (vertex_is_included(j, min_indices)) {
                    continue;
                }
                T value = matrix[i*size + j];
                if (value < min_values[i]) {
                    min_values[i] = value;
                    min_indices[i] = j;
                }
            }
        }
    }
}

template <typename T>
Solution<T> solve(const T *matrix, size_t size) {
    assert(size > 1);

    T total_cost = 0;

    std::vector<std::pair<size_t, size_t> > edges;
    std::vector<T> min_values(size, max_val<T>());
    std::vector<size_t> min_indices(size, size);
    size_t start_point = 1;

    // added first vertex
    size_t min_v = start_point;
    size_t min_vertex =
    std::min_element(&matrix[min_v*size], &matrix[min_v*size+size])
    - &matrix[min_v*size];
    min_values[min_v] = matrix[min_v*size+min_vertex];
    min_indices[min_v] = min_vertex;

    while (edges.size() < size-2) {
        // select min edge
        size_t min_u = start_point;
        T min_value = min_values[min_u];
        for (size_t i = min_u+1; i < size; ++i) {
            if (!vertex_is_included(i, min_indices)) {
                continue;
            }
            if (min_values[i] < min_value) {
                min_value = min_values[i];
                min_u = i;
            }
        }
        size_t min_v = min_indices[min_u];

        // add min edge
        edges.push_back(std::make_pair(min_u, min_v));
        min_indices[min_v] = min_v;

        // update min costs
        update_mins(matrix, size, min_v, &min_indices, &min_values);

        total_cost += min_value;
    }
    Solution<T> solution;
    solution.value = total_cost;
    solution.edges = edges;
    return solution;
}
}  // namespace Prim


template <typename T>
Solution<T> solve(const T *matrix, size_t size) {
    using std::swap;
    assert(size > 2);
    Solution<T> solution = Prim::solve(matrix, size);
    T min_values[] = {max_val<T>(), max_val<T>()};
    size_t min_indices[] = {size, size};

    for (size_t j = 0; j < size; ++j) {
        if (matrix[j] < min_values[0]) {
            min_values[1] = min_values[0];
            min_indices[1] = min_indices[0];
            min_values[0] = matrix[j];
            min_indices[0] = j;
        } else if (matrix[j] < min_values[1]) {
            min_values[1] = matrix[j];
            min_indices[1] = j;
        }
    }
    solution.value += min_values[0] + min_values[1];
    solution.edges.push_back(std::make_pair(0, min_indices[0]));
    solution.edges.push_back(std::make_pair(0, min_indices[1]));
    return solution;
}

}  // namespace MSOneTree
#endif  // TSP_SYMMETRIC_MS_ONE_TREE_INL_H_
