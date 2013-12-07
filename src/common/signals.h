// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_SIGNALS_H_
#define COMMON_SIGNALS_H_

#include <memory>

namespace Signals {

class InterruptingSignalGuard {
 public:
    InterruptingSignalGuard();

 private:
    InterruptingSignalGuard(const InterruptingSignalGuard &);
    InterruptingSignalGuard &operator=(const InterruptingSignalGuard &);

    class Impl;
    std::shared_ptr<Impl> impl_;
};

bool is_interrupted();
void set_intterrupting_signals();
}  // end namespace Signals

#endif  // COMMON_SIGNALS_H_
