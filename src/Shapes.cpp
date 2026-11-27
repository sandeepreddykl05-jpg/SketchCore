#include "sketchcore/Shapes.hpp"

#include <cmath>
#include <stdexcept>

#include "sketchcore/Intersections.hpp"

namespace sketchcore {

// ---- Line -------------------------------------------------------------------

bool Line::contains(Vec2 p, double tol) const { return distanceToSegment(p, a_, b_) <= tol; }

// ---- Circle -----------------------------------------------------------------

Circle::Circle(Vec2 center, double radius) : center_(center), radius_(radius) {
    if (!(radius >= 0.0) || !std::isfinite(radius))
        throw std::invalid_argument("Circle radius must be finite and non-negative");
}

void Circle::transform(const Matrix3& m) {
    center_ = m.apply(center_);
    radius_ *= std::sqrt(std::abs(m.linearDeterminant()));
}

// ---- Rectangle --------------------------------------------------------------

Rectangle Rectangle::fromCorners(Vec2 p, Vec2 q) {
    const Vec2 lo{std::min(p.x, q.x), std::min(p.y, q.y)};
    const Vec2 hi{std::max(p.x, q.x), std::max(p.y, q.y)};
    return Rectangle(lo, {hi.x - lo.x, 0.0}, {0.0, hi.y - lo.y});
}

BoundingBox Rectangle::boundingBox() const {
    BoundingBox box;
    for (Vec2 c : corners()) box.expand(c);
    return box;
}

bool Rectangle::contains(Vec2 p, double tol) const {
    const double lu = u_.length(), lv = v_.length();
    const double det = u_.cross(v_);

    // Degenerate (zero width/height or collinear edges): behaves like a segment/point.
    if (lu <= kEps || lv <= kEps || std::abs(det) <= kEps * lu * lv) {
        const auto c = corners();
        Vec2 best_a = c[0], best_b = c[0];
        double best = -1.0;
        for (std::size_t i = 0; i < 4; ++i)
            for (std::size_t j = i + 1; j < 4; ++j) {
                const double d = distance(c[i], c[j]);
                if (d > best) { best = d; best_a = c[i]; best_b = c[j]; }
            }
        return distanceToSegment(p, best_a, best_b) <= tol;
    }

    // Solve p - origin = a*u + b*v and check 0 <= a, b <= 1 (with tolerance in param space).
    const Vec2 w = p - origin_;
    const double a = w.cross(v_) / det;
    const double b = u_.cross(w) / det;
    const double ta = tol / lu, tb = tol / lv;
    return a >= -ta && a <= 1.0 + ta && b >= -tb && b <= 1.0 + tb;
}

// ---- Polygon ----------------------------------------------------------------

Polygon::Polygon(std::vector<Vec2> vertices) : pts_(std::move(vertices)) {
    if (pts_.size() < 3) throw std::invalid_argument("Polygon needs at least 3 vertices");
}

double Polygon::area() const {
    double twice = 0.0;
    for (std::size_t i = 0; i < pts_.size(); ++i)
        twice += pts_[i].cross(pts_[(i + 1) % pts_.size()]);
    return std::abs(twice) / 2.0;
}

double Polygon::perimeter() const {
    double sum = 0.0;
    for (std::size_t i = 0; i < pts_.size(); ++i) sum += distance(pts_[i], pts_[(i + 1) % pts_.size()]);
    return sum;
}

BoundingBox Polygon::boundingBox() const {
    BoundingBox box;
    for (Vec2 p : pts_) box.expand(p);
    return box;
}

bool Polygon::contains(Vec2 p, double tol) const {
    return pointInPolygon(p, pts_, tol) != PointLocation::Outside;
}

}  // namespace sketchcore
