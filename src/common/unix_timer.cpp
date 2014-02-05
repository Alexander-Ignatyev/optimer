// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#include "unix_timer.h"

#include <sys/time.h>

namespace {
double timeval_to_seconds(const timeval &tv) {
    static const double MICROSECS_IN_SEC = 1000000.0;
    return tv.tv_sec + tv.tv_usec / MICROSECS_IN_SEC;
}
}  //

UnixTimer::UnixTimer(): start_time_(0) {
    reset();
}

void UnixTimer::reset() {
    timeval tm;
    gettimeofday(&tm, NULL);
    start_time_ = timeval_to_seconds(tm);
}

double UnixTimer::elapsed_seconds() const {
    timeval tm;
    gettimeofday(&tm, NULL);
    double finish_time = timeval_to_seconds(tm);
    return finish_time - start_time_;
}
