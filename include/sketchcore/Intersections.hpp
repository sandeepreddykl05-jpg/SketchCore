#pragma once
#include <vector>

#include "sketchcore/Shapes.hpp"

namespace sketchcore {

enum class PointLocation { Outside, Boundary, Inside };

// Shortest distance from p to the segment [a, b] (handles a == b).
double distanceToSegment(Vec2 p, Vec2 a, Vec2 b);

// Ray-casting point-in-polygon with an explicit boundary test. Works for concave
// and degenerate (collinear) polygons. A point within `tol` of an edge is Boundary.
PointLocation pointInPolygon(Vec2 p, const std::vector<Vec2>& polygon, double tol = kEps);
inline PointLocation pointInPolygon(Vec2 p, const Polygon& polygon, double tol = kEps) {
    return pointInPolygon(p, polygon.vertices(), tol);
}

// Segment-segment intersection. Returns 0 points (disjoint or parallel), 1 point, or
// 2 points (collinear overlap: the endpoints of the overlapping part).
std::vector<Vec2> intersectLineLine(const Line& l1, const Line& l2, double eps = kEps);

// Segment-circle intersection: 0, 1 (tangent or one end inside) or 2 points.
std::vector<Vec2> intersectLineCircle(const Line& line, const Circle& circle, double eps = kEps);

struct CircleIntersection {
    std::vector<Vec2> points;  // 0, 1 (tangent) or 2 points
    bool coincident = false;   // identical circles: infinitely many intersections, points empty
};
CircleIntersection intersectCircleCircle(const Circle& c1, const Circle& c2, double eps = kEps);

}  // namespace sketchcore
