#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <chrono>

class Timer {
	std::chrono::steady_clock::time_point start_;
	
public:
	Timer() {
		reset();
	}
	
	void reset() {
		start_ = std::chrono::steady_clock::now();
	}
	
	double elapsed_seconds() const {
		using namespace std::chrono;

		return duration_cast<duration<double>>(steady_clock::now() - start_).count();
	}
};

#endif //__TIMER_HPP__
