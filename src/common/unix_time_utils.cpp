// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#include "unix_time_utils.h"

#include <sys/time.h>

double timeval_to_seconds(const timeval &tv) {
    static const double MICROSECS_IN_SEC = 1000000.0;
    return tv.tv_sec + tv.tv_usec / MICROSECS_IN_SEC;
}
