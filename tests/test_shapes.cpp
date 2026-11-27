#include <gtest/gtest.h>

#include <memory>

#include "sketchcore/SketchCore.hpp"

using namespace sketchcore;

// ---- Line ----
TEST(Line, LengthAreaPerimeterBox) {
    Line l({0, 0}, {3, 4});
    EXPECT_DOUBLE_EQ(l.length(), 5.0);
    EXPECT_DOUBLE_EQ(l.area(), 0.0);
    EXPECT_DOUBLE_EQ(l.perimeter(), 5.0);
    EXPECT_DOUBLE_EQ(l.boundingBox().width(), 3.0);
    EXPECT_DOUBLE_EQ(l.boundingBox().height(), 4.0);
}

TEST(Line, HitTestUsesTolerance) {
    Line l({0, 0}, {10, 0});
    EXPECT_TRUE(l.contains({5, 0}));
    EXPECT_FALSE(l.contains({5, 0.5}));
    EXPECT_TRUE(l.contains({5, 0.5}, 0.6));
    EXPECT_FALSE(l.contains({11, 0}, 0.5));  // past the end of the segment
    EXPECT_TRUE(l.contains({10.4, 0}, 0.5));
}

TEST(Line, ZeroLengthLineBehavesLikePoint) {
    Line l({2, 2}, {2, 2});
    EXPECT_DOUBLE_EQ(l.length(), 0.0);
    EXPECT_TRUE(l.contains({2, 2}));
    EXPECT_FALSE(l.contains({2, 3}));
}

// ---- Circle ----
TEST(Circle, AreaPerimeter) {
    Circle c({0, 0}, 2.0);
    EXPECT_NEAR(c.area(), kPi * 4.0, 1e-12);
    EXPECT_NEAR(c.perimeter(), 4.0 * kPi, 1e-12);
}

TEST(Circle, BoundingBoxAndHitTest) {
    Circle c({1, 1}, 2.0);
    EXPECT_TRUE(c.boundingBox().min.approxEquals({-1, -1}));
    EXPECT_TRUE(c.boundingBox().max.approxEquals({3, 3}));
    EXPECT_TRUE(c.contains({1, 1}));
    EXPECT_TRUE(c.contains({3, 1}));      // exactly on the boundary
    EXPECT_FALSE(c.contains({3.5, 1}));
    EXPECT_TRUE(c.contains({3.5, 1}, 0.5));
}

TEST(Circle, InvalidRadiusThrows) {
    EXPECT_THROW(Circle({0, 0}, -1.0), std::invalid_argument);
    EXPECT_THROW(Circle({0, 0}, std::nan("")), std::invalid_argument);
    EXPECT_NO_THROW(Circle({0, 0}, 0.0));
}

TEST(Circle, UniformScaleAndTranslate) {
    Circle c({1, 0}, 1.0);
    c.scale(3, 3);
    EXPECT_TRUE(c.center().approxEquals({3, 0}));
    EXPECT_NEAR(c.radius(), 3.0, 1e-12);
    c.translate({1, 1});
    EXPECT_TRUE(c.center().approxEquals({4, 1}));
    c.rotate(kPi / 2);  // rotation keeps the radius
    EXPECT_NEAR(c.radius(), 3.0, 1e-12);
}

// ---- Rectangle ----
TEST(Rectangle, FromCornersAnyOrder) {
    auto r = Rectangle::fromCorners({4, 5}, {1, 1});
    EXPECT_DOUBLE_EQ(r.area(), 12.0);
    EXPECT_DOUBLE_EQ(r.perimeter(), 14.0);
    EXPECT_TRUE(r.boundingBox().min.approxEquals({1, 1}));
    EXPECT_TRUE(r.boundingBox().max.approxEquals({4, 5}));
}

TEST(Rectangle, HitTest) {
    auto r = Rectangle::fromCorners({0, 0}, {4, 2});
    EXPECT_TRUE(r.contains({2, 1}));
    EXPECT_TRUE(r.contains({0, 0}));   // corner
    EXPECT_TRUE(r.contains({4, 1}));   // edge
    EXPECT_FALSE(r.contains({4.1, 1}));
    EXPECT_TRUE(r.contains({4.1, 1}, 0.2));
}

TEST(Rectangle, RotationKeepsAreaAndMovesHitArea) {
    auto r = Rectangle::fromCorners({0, 0}, {4, 2});
    r.rotate(kPi / 2);  // about the origin: now spans x in [-2,0], y in [0,4]
    EXPECT_NEAR(r.area(), 8.0, 1e-9);
    EXPECT_NEAR(r.perimeter(), 12.0, 1e-9);
    EXPECT_TRUE(r.contains({-1, 2}));
    EXPECT_FALSE(r.contains({1, 2}));
    EXPECT_NEAR(r.boundingBox().width(), 2.0, 1e-9);
    EXPECT_NEAR(r.boundingBox().height(), 4.0, 1e-9);
}

TEST(Rectangle, Rotated45DegreeHitTest) {
    auto r = Rectangle::fromCorners({-1, -1}, {1, 1});
    r.rotate(kPi / 4);  // diamond with vertices on the axes at distance sqrt(2)
    EXPECT_TRUE(r.contains({1.3, 0}));    // inside diamond, outside the original square
    EXPECT_FALSE(r.contains({1.2, 1.2}));  // inside original square corner region, outside diamond
}

TEST(Rectangle, DegenerateRectangleIsSegment) {
    auto r = Rectangle::fromCorners({0, 0}, {4, 0});  // zero height
    EXPECT_DOUBLE_EQ(r.area(), 0.0);
    EXPECT_TRUE(r.contains({2, 0}));
    EXPECT_FALSE(r.contains({2, 0.1}));
    auto point = Rectangle::fromCorners({1, 1}, {1, 1});
    EXPECT_TRUE(point.contains({1, 1}));
    EXPECT_FALSE(point.contains({1, 2}));
}

TEST(Rectangle, NonUniformScaleUpdatesArea) {
    auto r = Rectangle::fromCorners({0, 0}, {2, 3});
    r.scale(2, 0.5);
    EXPECT_NEAR(r.area(), 6.0, 1e-9);
    EXPECT_NEAR(r.boundingBox().width(), 4.0, 1e-9);
    EXPECT_NEAR(r.boundingBox().height(), 1.5, 1e-9);
}

// ---- Polygon ----
TEST(Polygon, FewerThanThreeVerticesThrows) {
    EXPECT_THROW(Polygon({{0, 0}, {1, 1}}), std::invalid_argument);
    EXPECT_THROW(Polygon({}), std::invalid_argument);
}

TEST(Polygon, SquareAreaPerimeterIgnoresWinding) {
    Polygon ccw({{0, 0}, {2, 0}, {2, 2}, {0, 2}});
    Polygon cw({{0, 0}, {0, 2}, {2, 2}, {2, 0}});
    EXPECT_DOUBLE_EQ(ccw.area(), 4.0);
    EXPECT_DOUBLE_EQ(cw.area(), 4.0);
    EXPECT_DOUBLE_EQ(ccw.perimeter(), 8.0);
}

TEST(Polygon, ConcaveLShape) {
    // L-shape: 3x3 square minus a 2x2 corner => area 5
    Polygon l({{0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 3}, {0, 3}});
    EXPECT_DOUBLE_EQ(l.area(), 5.0);
    EXPECT_TRUE(l.contains({0.5, 2.5}));
    EXPECT_TRUE(l.contains({2.5, 0.5}));
    EXPECT_FALSE(l.contains({2, 2}));  // in the notch
}

TEST(Polygon, DegenerateCollinearPolygonHasZeroAreaAndOnlyBoundaryHits) {
    Polygon flat({{0, 0}, {1, 0}, {2, 0}});
    EXPECT_DOUBLE_EQ(flat.area(), 0.0);
    EXPECT_TRUE(flat.contains({1, 0}));
    EXPECT_FALSE(flat.contains({1, 0.5}));
    EXPECT_FALSE(flat.contains({3, 0}));
}

TEST(Polygon, DuplicateVerticesDoNotBreakAreaOrHitTest) {
    Polygon p({{0, 0}, {2, 0}, {2, 0}, {2, 2}, {0, 2}});
    EXPECT_DOUBLE_EQ(p.area(), 4.0);
    EXPECT_TRUE(p.contains({1, 1}));
}

TEST(Polygon, TransformMovesVertices) {
    Polygon p({{0, 0}, {2, 0}, {0, 2}});
    p.translate({10, 10});
    EXPECT_TRUE(p.boundingBox().min.approxEquals({10, 10}));
    p.rotate(kPi, {10, 10});
    EXPECT_TRUE(p.boundingBox().max.approxEquals({10, 10}, 1e-9));
    EXPECT_NEAR(p.area(), 2.0, 1e-9);
}

// ---- Polymorphism / ownership ----
TEST(Shape, CloneIsDeepAndIndependent) {
    std::unique_ptr<Shape> original = std::make_unique<Circle>(Vec2{0, 0}, 1.0);
    std::unique_ptr<Shape> copy = original->clone();
    copy->translate({5, 5});
    EXPECT_TRUE(original->contains({0, 0}));
    EXPECT_FALSE(copy->contains({0, 0}));
    EXPECT_EQ(copy->name(), std::string("circle"));
}

TEST(Shape, PolymorphicAreaThroughBasePointer) {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Line>(Vec2{0, 0}, Vec2{1, 0}));
    shapes.push_back(std::make_unique<Circle>(Vec2{0, 0}, 1.0));
    shapes.push_back(std::make_unique<Rectangle>(Rectangle::fromCorners({0, 0}, {2, 2})));
    shapes.push_back(std::make_unique<Polygon>(std::vector<Vec2>{{0, 0}, {1, 0}, {0, 1}}));
    double total = 0;
    for (const auto& s : shapes) total += s->area();
    EXPECT_NEAR(total, 0.0 + kPi + 4.0 + 0.5, 1e-12);
}
