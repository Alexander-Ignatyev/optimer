// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_STATS_H_
#define COMMON_STATS_H_

#include <cstddef>

namespace Stats {

double mean(const double *values, size_t size);
double variance(const double *values, size_t size);
double stddev(const double *values, size_t size);

double max(const double *values, size_t size);
double min(const double *values, size_t size);

}  // end namespace Stats

#endif  // COMMON_STATS_H_
