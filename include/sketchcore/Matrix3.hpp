#pragma once
#include <array>
#include <cmath>
#include <optional>

#include "sketchcore/Vec2.hpp"

namespace sketchcore {

// 3x3 row-major matrix in homogeneous 2D coordinates (column-vector convention).
// `A * B` applies B first, then A. Only affine transforms (bottom row 0 0 1) are
// produced by the factory functions.
class Matrix3 {
public:
    Matrix3() : m_{1, 0, 0, 0, 1, 0, 0, 0, 1} {}
    Matrix3(double m00, double m01, double m02,
            double m10, double m11, double m12,
            double m20, double m21, double m22)
        : m_{m00, m01, m02, m10, m11, m12, m20, m21, m22} {}

    static Matrix3 identity() { return Matrix3(); }
    static Matrix3 translation(double dx, double dy) {
        return {1, 0, dx, 0, 1, dy, 0, 0, 1};
    }
    static Matrix3 rotation(double radians) {
        const double c = std::cos(radians), s = std::sin(radians);
        return {c, -s, 0, s, c, 0, 0, 0, 1};
    }
    static Matrix3 rotation(double radians, Vec2 pivot) {
        return translation(pivot.x, pivot.y) * rotation(radians) * translation(-pivot.x, -pivot.y);
    }
    static Matrix3 scaling(double sx, double sy) { return {sx, 0, 0, 0, sy, 0, 0, 0, 1}; }
    static Matrix3 scaling(double sx, double sy, Vec2 pivot) {
        return translation(pivot.x, pivot.y) * scaling(sx, sy) * translation(-pivot.x, -pivot.y);
    }

    double at(int row, int col) const { return m_[static_cast<std::size_t>(row * 3 + col)]; }

    Matrix3 operator*(const Matrix3& o) const {
        Matrix3 r;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) {
                double sum = 0.0;
                for (int k = 0; k < 3; ++k) sum += at(i, k) * o.at(k, j);
                r.m_[static_cast<std::size_t>(i * 3 + j)] = sum;
            }
        return r;
    }

    // Transforms a point (translation applies).
    Vec2 apply(Vec2 p) const {
        return {at(0, 0) * p.x + at(0, 1) * p.y + at(0, 2),
                at(1, 0) * p.x + at(1, 1) * p.y + at(1, 2)};
    }
    // Transforms a direction/offset (translation ignored).
    Vec2 applyVector(Vec2 v) const {
        return {at(0, 0) * v.x + at(0, 1) * v.y, at(1, 0) * v.x + at(1, 1) * v.y};
    }

    double determinant() const {
        return at(0, 0) * (at(1, 1) * at(2, 2) - at(1, 2) * at(2, 1)) -
               at(0, 1) * (at(1, 0) * at(2, 2) - at(1, 2) * at(2, 0)) +
               at(0, 2) * (at(1, 0) * at(2, 1) - at(1, 1) * at(2, 0));
    }
    // Determinant of the upper-left 2x2 block: the area scale factor of the transform.
    double linearDeterminant() const { return at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0); }

    // Returns std::nullopt for singular matrices (e.g. scaling by 0).
    std::optional<Matrix3> inverse() const {
        const double det = determinant();
        if (std::abs(det) < 1e-12) return std::nullopt;
        const double d = 1.0 / det;
        return Matrix3{
            (at(1, 1) * at(2, 2) - at(1, 2) * at(2, 1)) * d,
            (at(0, 2) * at(2, 1) - at(0, 1) * at(2, 2)) * d,
            (at(0, 1) * at(1, 2) - at(0, 2) * at(1, 1)) * d,
            (at(1, 2) * at(2, 0) - at(1, 0) * at(2, 2)) * d,
            (at(0, 0) * at(2, 2) - at(0, 2) * at(2, 0)) * d,
            (at(0, 2) * at(1, 0) - at(0, 0) * at(1, 2)) * d,
            (at(1, 0) * at(2, 1) - at(1, 1) * at(2, 0)) * d,
            (at(0, 1) * at(2, 0) - at(0, 0) * at(2, 1)) * d,
            (at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0)) * d};
    }

    bool approxEquals(const Matrix3& o, double eps = kEps) const {
        for (std::size_t i = 0; i < 9; ++i)
            if (!nearlyEqual(m_[i], o.m_[i], eps)) return false;
        return true;
    }

private:
    std::array<double, 9> m_;
};

}  // namespace sketchcore
