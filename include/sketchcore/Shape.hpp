#pragma once
#include <memory>
#include <string>

#include "sketchcore/BoundingBox.hpp"
#include "sketchcore/Matrix3.hpp"

namespace sketchcore {

// Abstract base of the polymorphic shape hierarchy. Shapes are value-like (deep
// copy via clone()) and owned through std::unique_ptr<Shape>.
class Shape {
public:
    virtual ~Shape() = default;

    virtual std::string name() const = 0;
    virtual double area() const = 0;
    virtual double perimeter() const = 0;
    virtual BoundingBox boundingBox() const = 0;

    // Hit-test. For filled shapes: true if p is inside or on the boundary.
    // For a Line: true if p lies within `tol` of the segment.
    // `tol` is also added to the boundary so callers can implement "click slop".
    virtual bool contains(Vec2 p, double tol = kEps) const = 0;

    virtual void transform(const Matrix3& m) = 0;
    virtual std::unique_ptr<Shape> clone() const = 0;

    void translate(Vec2 d) { transform(Matrix3::translation(d.x, d.y)); }
    void rotate(double radians, Vec2 pivot = {}) { transform(Matrix3::rotation(radians, pivot)); }
    void scale(double sx, double sy, Vec2 pivot = {}) { transform(Matrix3::scaling(sx, sy, pivot)); }

protected:
    // Protected copy operations prevent slicing through Shape references.
    Shape() = default;
    Shape(const Shape&) = default;
    Shape& operator=(const Shape&) = default;
};

}  // namespace sketchcore
