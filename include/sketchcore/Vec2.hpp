#pragma once
#include <cmath>

#include "sketchcore/Tolerance.hpp"

namespace sketchcore {

struct Vec2 {
    double x = 0.0;
    double y = 0.0;

    constexpr Vec2() = default;
    constexpr Vec2(double x_, double y_) : x(x_), y(y_) {}

    constexpr Vec2 operator+(Vec2 o) const { return {x + o.x, y + o.y}; }
    constexpr Vec2 operator-(Vec2 o) const { return {x - o.x, y - o.y}; }
    constexpr Vec2 operator-() const { return {-x, -y}; }
    constexpr Vec2 operator*(double s) const { return {x * s, y * s}; }
    constexpr Vec2 operator/(double s) const { return {x / s, y / s}; }
    Vec2& operator+=(Vec2 o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(Vec2 o) { x -= o.x; y -= o.y; return *this; }

    constexpr double dot(Vec2 o) const { return x * o.x + y * o.y; }
    // z-component of the 3D cross product; > 0 when o is counter-clockwise from *this.
    constexpr double cross(Vec2 o) const { return x * o.y - y * o.x; }
    constexpr double lengthSquared() const { return x * x + y * y; }
    double length() const { return std::sqrt(lengthSquared()); }

    // Returns the zero vector for a (near-)zero input instead of NaN.
    Vec2 normalized() const {
        const double len = length();
        return len > 0.0 ? Vec2{x / len, y / len} : Vec2{};
    }

    bool approxEquals(Vec2 o, double eps = kEps) const {
        return nearlyEqual(x, o.x, eps) && nearlyEqual(y, o.y, eps);
    }
};

constexpr Vec2 operator*(double s, Vec2 v) { return v * s; }
inline double distance(Vec2 a, Vec2 b) { return (a - b).length(); }

}  // namespace sketchcore
