// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#include "resource_utilization.h"
#include "unix_time_utils.h"

ResourceUtilization::ResourceUtilization(): error_status_(true) {
    refresh();
}

void ResourceUtilization::refresh() {
    error_status_ = (getrusage(RUSAGE_SELF, &usage_) == 0);
}

double ResourceUtilization::user_time() const {
    return timeval_to_seconds(usage_.ru_utime);
}

double ResourceUtilization::system_time() const {
    return timeval_to_seconds(usage_.ru_stime);
}
