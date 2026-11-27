#pragma once
#include <array>
#include <vector>

#include "sketchcore/Shape.hpp"

namespace sketchcore {

// A line *segment* from a() to b().
class Line final : public Shape {
public:
    Line(Vec2 a, Vec2 b) : a_(a), b_(b) {}

    Vec2 a() const { return a_; }
    Vec2 b() const { return b_; }
    double length() const { return distance(a_, b_); }

    std::string name() const override { return "line"; }
    double area() const override { return 0.0; }
    double perimeter() const override { return length(); }  // a segment's "perimeter" is its length
    BoundingBox boundingBox() const override { return BoundingBox(a_, b_); }
    bool contains(Vec2 p, double tol = kEps) const override;
    void transform(const Matrix3& m) override { a_ = m.apply(a_); b_ = m.apply(b_); }
    std::unique_ptr<Shape> clone() const override { return std::make_unique<Line>(*this); }

private:
    Vec2 a_, b_;
};

class Circle final : public Shape {
public:
    // Throws std::invalid_argument for a negative or non-finite radius.
    Circle(Vec2 center, double radius);

    Vec2 center() const { return center_; }
    double radius() const { return radius_; }

    std::string name() const override { return "circle"; }
    double area() const override { return kPi * radius_ * radius_; }
    double perimeter() const override { return 2.0 * kPi * radius_; }
    BoundingBox boundingBox() const override {
        return BoundingBox({center_.x - radius_, center_.y - radius_},
                           {center_.x + radius_, center_.y + radius_});
    }
    bool contains(Vec2 p, double tol = kEps) const override {
        return distance(p, center_) <= radius_ + tol;
    }
    // A circle stays a circle only under similarity transforms. For general affine
    // maps the radius is scaled by sqrt(|det|) (the area-preserving average).
    void transform(const Matrix3& m) override;
    std::unique_ptr<Shape> clone() const override { return std::make_unique<Circle>(*this); }

private:
    Vec2 center_;
    double radius_;
};

// Rectangle stored as origin + two edge vectors (u, v). This representation is
// closed under *every* affine transform, so rotation works exactly. Under shear or
// non-uniform scale after rotation it becomes a parallelogram.
class Rectangle final : public Shape {
public:
    Rectangle(Vec2 origin, Vec2 u, Vec2 v) : origin_(origin), u_(u), v_(v) {}
    // Axis-aligned rectangle spanning two opposite corners (any order).
    static Rectangle fromCorners(Vec2 p, Vec2 q);

    Vec2 origin() const { return origin_; }
    Vec2 u() const { return u_; }
    Vec2 v() const { return v_; }
    std::array<Vec2, 4> corners() const {
        return {origin_, origin_ + u_, origin_ + u_ + v_, origin_ + v_};
    }

    std::string name() const override { return "rectangle"; }
    double area() const override { return std::abs(u_.cross(v_)); }
    double perimeter() const override { return 2.0 * (u_.length() + v_.length()); }
    BoundingBox boundingBox() const override;
    bool contains(Vec2 p, double tol = kEps) const override;
    void transform(const Matrix3& m) override {
        origin_ = m.apply(origin_);
        u_ = m.applyVector(u_);
        v_ = m.applyVector(v_);
    }
    std::unique_ptr<Shape> clone() const override { return std::make_unique<Rectangle>(*this); }

private:
    Vec2 origin_, u_, v_;
};

// Simple polygon (implicitly closed). Concave polygons are supported.
// Area uses the shoelace formula, so it is only meaningful for non-self-intersecting input.
class Polygon final : public Shape {
public:
    // Throws std::invalid_argument if fewer than 3 vertices are given.
    explicit Polygon(std::vector<Vec2> vertices);

    const std::vector<Vec2>& vertices() const { return pts_; }

    std::string name() const override { return "polygon"; }
    double area() const override;
    double perimeter() const override;
    BoundingBox boundingBox() const override;
    bool contains(Vec2 p, double tol = kEps) const override;
    void transform(const Matrix3& m) override {
        for (auto& p : pts_) p = m.apply(p);
    }
    std::unique_ptr<Shape> clone() const override { return std::make_unique<Polygon>(*this); }

private:
    std::vector<Vec2> pts_;
};

}  // namespace sketchcore
