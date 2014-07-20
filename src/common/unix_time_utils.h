// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_UNIX_TIME_UTILS_H_
#define COMMON_UNIX_TIME_UTILS_H_

struct timeval;

double timeval_to_seconds(const timeval &tv);

#endif  // COMMON_UNIX_TIME_UTILS_H_
