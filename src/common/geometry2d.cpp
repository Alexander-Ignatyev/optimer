// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "geometry2d.h"

#include <cmath>

namespace Geometry2D {
    double distance(const Point &lhs, const Point &rhs) {
        double x = lhs.x - rhs.x;
        double y = lhs.y - rhs.y;
        return sqrt(x*x + y*y);
    }
}  // namespace Geometry2D
