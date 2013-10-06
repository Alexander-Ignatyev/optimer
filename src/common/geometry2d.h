// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_GEOMETRY2D_H_
#define COMMON_GEOMETRY2D_H_

namespace Geometry2D {
    struct Point {
        double x;
        double y;
    };

    double distance(const Point &lhs, const Point &rhs);

}  // namespace Geometry2D

#endif  // COMMON_GEOMETRY2D_H_
