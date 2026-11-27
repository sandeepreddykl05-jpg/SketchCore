#pragma once
#include <algorithm>
#include <limits>

#include "sketchcore/Vec2.hpp"

namespace sketchcore {

// Axis-aligned bounding box. A default-constructed box is empty.
struct BoundingBox {
    Vec2 min{std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    Vec2 max{-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()};

    BoundingBox() = default;
    BoundingBox(Vec2 a, Vec2 b)
        : min{std::min(a.x, b.x), std::min(a.y, b.y)}, max{std::max(a.x, b.x), std::max(a.y, b.y)} {}

    bool isEmpty() const { return min.x > max.x || min.y > max.y; }
    double width() const { return isEmpty() ? 0.0 : max.x - min.x; }
    double height() const { return isEmpty() ? 0.0 : max.y - min.y; }
    Vec2 center() const { return {(min.x + max.x) / 2, (min.y + max.y) / 2}; }

    void expand(Vec2 p) {
        min = {std::min(min.x, p.x), std::min(min.y, p.y)};
        max = {std::max(max.x, p.x), std::max(max.y, p.y)};
    }
    void expand(const BoundingBox& o) {
        if (o.isEmpty()) return;
        expand(o.min);
        expand(o.max);
    }
    bool contains(Vec2 p, double tol = 0.0) const {
        return !isEmpty() && p.x >= min.x - tol && p.x <= max.x + tol && p.y >= min.y - tol &&
               p.y <= max.y + tol;
    }
    bool intersects(const BoundingBox& o) const {
        return !isEmpty() && !o.isEmpty() && min.x <= o.max.x && max.x >= o.min.x &&
               min.y <= o.max.y && max.y >= o.min.y;
    }
};

}  // namespace sketchcore
