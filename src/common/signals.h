// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_SIGNALS_H_
#define COMMON_SIGNALS_H_

namespace Signals {

class InterruptingSignalGuard {
 public:
    InterruptingSignalGuard();
    ~InterruptingSignalGuard();

 private:
    InterruptingSignalGuard(const InterruptingSignalGuard &);
    InterruptingSignalGuard &operator=(const InterruptingSignalGuard &);

    class Impl;
    Impl *impl_;
};

bool is_interrupted();
void set_intterrupting_signals();
}  // end namespace Signals

#endif  // COMMON_SIGNALS_H_
