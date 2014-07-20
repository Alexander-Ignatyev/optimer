// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_RESOURCE_UTILIZATION_H_
#define COMMON_RESOURCE_UTILIZATION_H_

#include <sys/resource.h>

class ResourceUtilization {
 public:
    ResourceUtilization();
    void refresh();
    double user_time() const;
    double system_time() const;
    bool is_error_status() const {
        return error_status_;
    }

 private:
    bool error_status_;
    rusage usage_;
};

#endif  // COMMON_RESOURCE_UTILIZATION_H_
