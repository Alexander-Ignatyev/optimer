// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_LOG_H_
#define COMMON_LOG_H_

#ifdef USE_G2LOG
#include <g2log.h>
#include <g2logworker.h>

#define init_logger(prefix) \
    g2LogWorker g2log(prefix, ""); \
    g2::initializeLogging(&g2log);

#else
#include <iostream>
struct nullstream {};

template <typename T>
nullstream & operator<<(nullstream & s, T const &) {
    return s;
}

nullstream & operator<<(nullstream & s, std::ostream &(std::ostream&));

extern nullstream logstream;

#define LOG(LEVEL) logstream

#define CHECK(BOOL_EXPR) if (0) logstream

#define LOG_IF(LEVEL, BOOL_EXPR) if (0) logstream

#define CHECK_F(BOOL_EXPR, printf_like_message, ...)

#define init_logger(prefix)
#endif

#endif  // COMMON_LOG_H_
