// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#include "stats.h"

#include <cmath>
#include <limits>

namespace Stats {
double mean(const double *values, size_t size) {
    double sum = 0;
    for (unsigned i = 0; i < size; ++i) {
        sum += values[i];
    }
    return sum/size;
}

double variance(const double *values, size_t size) {
    double mean_value = mean(values, size);
    double ssum = 0;
    for (size_t i = 0; i < size; ++i) {
        ssum += (mean_value-values[i])*(mean_value-values[i]);
    }
    return ssum/size;
}

double stddev(const double *values, size_t size) {
    return sqrt(variance(values, size));
}

double max(const double *values, size_t size) {
    // in case of size = 0
    double max_value = std::numeric_limits<double>::min();
    for (size_t i = 0; i < size; ++i) {
        if (max_value < values[i]) {
            max_value = values[i];
        }
    }
    return max_value;
}

double min(const double *values, size_t size) {
    // in case of size = 0
    double min_value = std::numeric_limits<double>::max();
    for (size_t i = 0; i < size; ++i) {
        if (min_value > values[i]) {
            min_value = values[i];
        }
    }
    return min_value;
}

}  // end namespace Stats
