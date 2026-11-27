#pragma once
#include <algorithm>
#include <cmath>

namespace sketchcore {

inline constexpr double kEps = 1e-9;
inline constexpr double kPi = 3.14159265358979323846;

// Relative+absolute comparison: values near 0 use an absolute tolerance,
// large values use a relative one.
inline bool nearlyEqual(double a, double b, double eps = kEps) {
    return std::abs(a - b) <= eps * std::max({1.0, std::abs(a), std::abs(b)});
}

inline bool nearlyZero(double a, double eps = kEps) { return std::abs(a) <= eps; }

}  // namespace sketchcore
