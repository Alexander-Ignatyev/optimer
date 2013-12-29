// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "log.h"

#ifndef USE_G2LOG

nullstream logstream;

nullstream & operator<<(nullstream & s, std::ostream &(std::ostream&)) {
    return s;
}

#endif
