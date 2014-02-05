// Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_UNIX_TIMER_H_
#define COMMON_UNIX_TIMER_H_

class UnixTimer {
 public:
    UnixTimer();
    void reset();
    double elapsed_seconds() const;

 private:
    double start_time_;
};

#endif  // COMMON_UNIX_TIMER_H_
