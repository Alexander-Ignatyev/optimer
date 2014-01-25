// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#include "signals.h"

#include <signal.h>

#include <iostream>
#include <map>

namespace Signals {

volatile sig_atomic_t g_interruped = 0;

namespace {
void set_interrupted(int /*signo*/) {
    g_interruped = 1;
}
}  // end namespace

class InterruptingSignalGuard::Impl {
 public:
    Impl() {
        g_interruped = 0;
        const int sig_nums[] = {SIGINT, SIGTERM};
        for (size_t i = 0; i < sizeof(sig_nums)/sizeof(sig_nums[0]); ++i) {
            struct sigaction action;
            struct sigaction old_action;
            action.sa_handler = set_interrupted;
            sigemptyset(&action.sa_mask);
            action.sa_flags = 0;

            if (sigaction(sig_nums[i], &action, &old_action) < 0) {
                std::cerr << "InterruptingSignalGuard: sigaction error"
                << std::endl;
            } else {
                old_handlers_[sig_nums[i]] = old_action;
            }
        }
    }

    ~Impl() {
        std::map<int, struct sigaction>::iterator pos
            , end = old_handlers_.end();
        for (pos = old_handlers_.begin(); pos != end; ++pos) {
            if (sigaction(pos->first, &pos->second, 0) < 0) {
                std::cerr << "~InterruptingSignalGuard: sigaction error"
                    << std::endl;
            }
        }
    }

 private:
    std::map<int, struct sigaction> old_handlers_;
};

InterruptingSignalGuard::InterruptingSignalGuard()
    : impl_(new InterruptingSignalGuard::Impl()) {
}

InterruptingSignalGuard::~InterruptingSignalGuard() {
    delete impl_;
}

bool is_interrupted() {
    return g_interruped != 0;
}

}  // end namespace Signals
