#include "sketchcore/Intersections.hpp"

#include <algorithm>
#include <cmath>

namespace sketchcore {

namespace {
double clamp01(double t) { return std::min(1.0, std::max(0.0, t)); }
}  // namespace

double distanceToSegment(Vec2 p, Vec2 a, Vec2 b) {
    const Vec2 d = b - a;
    const double l2 = d.lengthSquared();
    if (l2 == 0.0) return distance(p, a);
    const double t = clamp01((p - a).dot(d) / l2);
    return distance(p, a + d * t);
}

PointLocation pointInPolygon(Vec2 p, const std::vector<Vec2>& poly, double tol) {
    const std::size_t n = poly.size();
    if (n == 0) return PointLocation::Outside;

    for (std::size_t i = 0; i < n; ++i)
        if (distanceToSegment(p, poly[i], poly[(i + 1) % n]) <= tol) return PointLocation::Boundary;

    // Even-odd rule with the half-open (a.y > p.y) != (b.y > p.y) test, so a ray passing
    // exactly through a vertex is counted once, not twice.
    bool inside = false;
    for (std::size_t i = 0, j = n - 1; i < n; j = i++) {
        const Vec2 a = poly[i], b = poly[j];
        if ((a.y > p.y) != (b.y > p.y)) {
            const double xCross = (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x;
            if (p.x < xCross) inside = !inside;
        }
    }
    return inside ? PointLocation::Inside : PointLocation::Outside;
}

std::vector<Vec2> intersectLineLine(const Line& l1, const Line& l2, double eps) {
    const Vec2 p = l1.a(), r = l1.b() - l1.a();
    const Vec2 q = l2.a(), s = l2.b() - l2.a();
    const double rr = r.lengthSquared(), ss = s.lengthSquared();

    // Degenerate (zero-length) segments behave like points.
    if (rr <= eps * eps && ss <= eps * eps) return distance(p, q) <= eps ? std::vector<Vec2>{p} : std::vector<Vec2>{};
    if (rr <= eps * eps) return l2.contains(p, eps) ? std::vector<Vec2>{p} : std::vector<Vec2>{};
    if (ss <= eps * eps) return l1.contains(q, eps) ? std::vector<Vec2>{q} : std::vector<Vec2>{};

    const double lr = std::sqrt(rr), ls = std::sqrt(ss);
    const double denom = r.cross(s);
    const Vec2 qp = q - p;

    if (std::abs(denom) <= eps * lr * ls) {
        // Parallel. Disjoint unless collinear.
        if (std::abs(qp.cross(r)) / lr > eps * std::max(1.0, qp.length())) return {};
        // Collinear: overlap of [0,1] with the parameter interval of l2 along l1.
        const double t0 = qp.dot(r) / rr;
        const double t1 = (l2.b() - p).dot(r) / rr;
        const double lo = std::max(0.0, std::min(t0, t1));
        const double hi = std::min(1.0, std::max(t0, t1));
        const double te = eps / lr;
        if (lo > hi + te) return {};
        if (hi - lo <= te) return {p + r * clamp01((lo + hi) / 2.0)};
        return {p + r * lo, p + r * hi};
    }

    const double t = qp.cross(s) / denom;
    const double u = qp.cross(r) / denom;
    const double te = eps / lr, ue = eps / ls;
    if (t < -te || t > 1.0 + te || u < -ue || u > 1.0 + ue) return {};
    return {p + r * clamp01(t)};
}

std::vector<Vec2> intersectLineCircle(const Line& line, const Circle& circle, double eps) {
    const Vec2 a = line.a(), d = line.b() - line.a();
    const Vec2 c = circle.center();
    const double r = circle.radius();
    const double len = d.length();

    if (len <= eps)
        return nearlyEqual(distance(a, c), r, eps) ? std::vector<Vec2>{a} : std::vector<Vec2>{};

    // Closest point on the infinite line to the circle's center.
    const double tc = (c - a).dot(d) / (len * len);
    const double dist = distance(a + d * tc, c);
    const double te = eps / len;
    auto inRange = [&](double t) { return t >= -te && t <= 1.0 + te; };

    if (dist > r && !nearlyEqual(dist, r, eps)) return {};  // misses entirely

    if (nearlyEqual(dist, r, eps)) {  // tangent
        if (!inRange(tc)) return {};
        return {a + d * clamp01(tc)};
    }

    const double dt = std::sqrt(r * r - dist * dist) / len;
    std::vector<Vec2> out;
    for (double t : {tc - dt, tc + dt})
        if (inRange(t)) out.push_back(a + d * clamp01(t));
    return out;
}

CircleIntersection intersectCircleCircle(const Circle& c1, const Circle& c2, double eps) {
    CircleIntersection result;
    const Vec2 d = c2.center() - c1.center();
    const double dist = d.length();
    const double r1 = c1.radius(), r2 = c2.radius();

    if (dist <= eps) {  // concentric
        result.coincident = nearlyEqual(r1, r2, eps);
        return result;
    }

    const Vec2 dir = d / dist;
    const double sum = r1 + r2, diff = std::abs(r1 - r2);

    if (nearlyEqual(dist, sum, eps)) {  // externally tangent
        result.points.push_back(c1.center() + dir * r1);
        return result;
    }
    if (nearlyEqual(dist, diff, eps)) {  // internally tangent
        result.points.push_back(r1 >= r2 ? c1.center() + dir * r1 : c2.center() - dir * r2);
        return result;
    }
    if (dist > sum || dist < diff) return result;  // separate or nested

    const double a = (dist * dist + r1 * r1 - r2 * r2) / (2.0 * dist);
    const double h = std::sqrt(std::max(0.0, r1 * r1 - a * a));
    const Vec2 mid = c1.center() + dir * a;
    const Vec2 perp{-dir.y, dir.x};
    result.points = {mid + perp * h, mid - perp * h};
    return result;
}

}  // namespace sketchcore
