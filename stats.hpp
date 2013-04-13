#ifndef __STATS_HPP__
#define __STATS_HPP__

#include <iostream>
struct Stats {
	size_t branches;
	size_t sets_generated;
	size_t sets_constrained_by_record;
	size_t sets_sent;
	size_t sets_received;
	double seconds;
	
	Stats() {
		clear();
	}

	void clear() {
		branches = 0;
		sets_generated = 0;
		sets_constrained_by_record = 0;
		sets_sent = 0;
		sets_received = 0;
		seconds = 0.0;
	}
};

std::ostream & operator << (std::ostream &os, const Stats &stats) {
	os << "Branches: " << stats.branches << std::endl;
	os << "Generated sets: " << stats.sets_generated << std::endl;
	os << "Constrained by record: " << stats.sets_constrained_by_record << std::endl;
	os << "Number of sets were sent: " << stats.sets_sent << std::endl;
	os << "Number of sets were received: " << stats.sets_received << std::endl;
	os << "Time: " << stats.seconds << " secs." << std::endl;
	return os;
}

#endif //__STATS_HPP__
