// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_STATS_H_
#define SRC_STATS_H_

#include <iosfwd>

struct Stats {
    Stats();
    void clear();

    size_t branches;
    size_t sets_generated;
    size_t sets_constrained_by_record;
    size_t sets_sent;
    size_t sets_received;
    double seconds;
};

std::ostream & operator << (std::ostream &os, const Stats &stats);

#endif  // SRC_STATS_H_
