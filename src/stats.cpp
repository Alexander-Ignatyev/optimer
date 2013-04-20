// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "stats.h"

#include <fstream>

Stats::Stats() {
    clear();
}

void Stats::clear() {
    branches = 0;
    sets_generated = 0;
    sets_constrained_by_record = 0;
    sets_sent = 0;
    sets_received = 0;
    seconds = 0.0;
}

std::ostream & operator << (std::ostream &os, const Stats &stats) {
    os << "Branches: " << stats.branches << std::endl;
    os << "Generated sets: " << stats.sets_generated << std::endl;
    os << "Constrained by record: ";
    os << stats.sets_constrained_by_record << std::endl;
    os << "Number of sets were sent: " << stats.sets_sent << std::endl;
    os << "Number of sets were received: " << stats.sets_received << std::endl;
    os << "Time: " << stats.seconds << " secs." << std::endl;
    return os;
}
