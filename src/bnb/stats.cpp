// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "stats.h"

#include <fstream>

Stats::Stats() {
    clear();
}

void Stats::clear() {
    branches = 0;
    sets_generated = 0;
    bound_problems_solved = 0;
    sets_constrained_by_record = 0;
    sets_sent = 0;
    sets_received = 0;
    seconds = 0.0;
}

Stats &Stats::operator+=(const Stats &rhs) {
    branches += rhs.branches;
    sets_generated += rhs.sets_generated;
    bound_problems_solved += rhs.bound_problems_solved;
    sets_constrained_by_record += rhs.sets_constrained_by_record;
    sets_sent += rhs.sets_sent;
    sets_received += rhs.sets_received;
    seconds += rhs.seconds;
    return *this;
}

Stats &Stats::operator /= (size_t value) {
    branches /= value;
    sets_generated /= value;
    bound_problems_solved /= value;
    sets_constrained_by_record /= value;
    sets_sent /= value;
    sets_received /= value;
    seconds /= value;
    return *this;
}

Stats operator / (Stats lhs, double value) {
    return lhs /= value;
}

std::ostream & operator << (std::ostream &os, const Stats &stats) {
    os << "Branches: " << stats.branches << std::endl;
    os << "Generated sets: " << stats.sets_generated << std::endl;
    os << "Bound problems solved: " << stats.bound_problems_solved << std::endl;
    os << "Constrained by record: ";
    os << stats.sets_constrained_by_record << std::endl;
    os << "Number of sets were sent: " << stats.sets_sent << std::endl;
    os << "Number of sets were received: " << stats.sets_received << std::endl;
    os << "Time: " << stats.seconds << " secs." << std::endl;
    return os;
}
