#include <gtest/gtest.h>

#include "sketchcore/SketchCore.hpp"

using namespace sketchcore;

// ---- line-line ----
TEST(LineLine, CrossingSegments) {
    auto pts = intersectLineLine(Line({0, 0}, {4, 4}), Line({0, 4}, {4, 0}));
    ASSERT_EQ(pts.size(), 1u);
    EXPECT_TRUE(pts[0].approxEquals({2, 2}));
}

TEST(LineLine, ParallelLinesDoNotIntersect) {
    EXPECT_TRUE(intersectLineLine(Line({0, 0}, {1, 0}), Line({0, 1}, {1, 1})).empty());
}

TEST(LineLine, NearlyParallelWithinToleranceTreatedAsParallel) {
    // Slope difference of 1e-13 is below tolerance: no spurious far-away intersection.
    EXPECT_TRUE(intersectLineLine(Line({0, 0}, {1, 0}), Line({0, 1}, {1, 1 + 1e-13})).empty());
}

TEST(LineLine, CollinearOverlapReturnsOverlapEndpoints) {
    auto pts = intersectLineLine(Line({0, 0}, {2, 0}), Line({1, 0}, {3, 0}));
    ASSERT_EQ(pts.size(), 2u);
    EXPECT_TRUE(pts[0].approxEquals({1, 0}));
    EXPECT_TRUE(pts[1].approxEquals({2, 0}));
}

TEST(LineLine, CollinearDisjointSegments) {
    EXPECT_TRUE(intersectLineLine(Line({0, 0}, {1, 0}), Line({2, 0}, {3, 0})).empty());
}

TEST(LineLine, CollinearTouchingAtEndpointGivesOnePoint) {
    auto pts = intersectLineLine(Line({0, 0}, {1, 0}), Line({1, 0}, {2, 0}));
    ASSERT_EQ(pts.size(), 1u);
    EXPECT_TRUE(pts[0].approxEquals({1, 0}));
}

TEST(LineLine, TJunctionAtEndpoint) {
    auto pts = intersectLineLine(Line({0, 0}, {4, 0}), Line({2, 0}, {2, 3}));
    ASSERT_EQ(pts.size(), 1u);
    EXPECT_TRUE(pts[0].approxEquals({2, 0}));
}

TEST(LineLine, LinesWouldCrossButSegmentsAreTooShort) {
    EXPECT_TRUE(intersectLineLine(Line({0, 0}, {1, 1}), Line({0, 4}, {1, 3})).empty());
}

TEST(LineLine, ZeroLengthSegmentsActLikePoints) {
    EXPECT_EQ(intersectLineLine(Line({1, 1}, {1, 1}), Line({0, 0}, {2, 2})).size(), 1u);
    EXPECT_TRUE(intersectLineLine(Line({1, 2}, {1, 2}), Line({0, 0}, {2, 2})).empty());
    EXPECT_EQ(intersectLineLine(Line({1, 1}, {1, 1}), Line({1, 1}, {1, 1})).size(), 1u);
}

TEST(LineLine, IntersectionIsSymmetric) {
    Line a({0, 0}, {5, 3}), b({1, 4}, {4, -1});
    auto p = intersectLineLine(a, b), q = intersectLineLine(b, a);
    ASSERT_EQ(p.size(), 1u);
    ASSERT_EQ(q.size(), 1u);
    EXPECT_TRUE(p[0].approxEquals(q[0], 1e-9));
}

// ---- line-circle ----
TEST(LineCircle, SecantThroughCenterGivesTwoPoints) {
    auto pts = intersectLineCircle(Line({-5, 0}, {5, 0}), Circle({0, 0}, 1));
    ASSERT_EQ(pts.size(), 2u);
    EXPECT_TRUE(pts[0].approxEquals({-1, 0}, 1e-9));
    EXPECT_TRUE(pts[1].approxEquals({1, 0}, 1e-9));
}

TEST(LineCircle, TangentLineGivesOnePoint) {
    auto pts = intersectLineCircle(Line({-5, 1}, {5, 1}), Circle({0, 0}, 1));
    ASSERT_EQ(pts.size(), 1u);
    EXPECT_TRUE(pts[0].approxEquals({0, 1}, 1e-9));
}

TEST(LineCircle, NearTangentWithinToleranceStillTangent) {
    auto pts = intersectLineCircle(Line({-5, 1 + 1e-12}, {5, 1 + 1e-12}), Circle({0, 0}, 1));
    EXPECT_EQ(pts.size(), 1u);
}

TEST(LineCircle, MissReturnsNothing) {
    EXPECT_TRUE(intersectLineCircle(Line({-5, 2}, {5, 2}), Circle({0, 0}, 1)).empty());
}

TEST(LineCircle, SegmentEndingBeforeCircleMisses) {
    EXPECT_TRUE(intersectLineCircle(Line({-5, 0}, {-2, 0}), Circle({0, 0}, 1)).empty());
}

TEST(LineCircle, SegmentFullyInsideCircleMisses) {
    EXPECT_TRUE(intersectLineCircle(Line({-0.5, 0}, {0.5, 0}), Circle({0, 0}, 1)).empty());
}

TEST(LineCircle, SegmentStartingInsideGivesOnePoint) {
    auto pts = intersectLineCircle(Line({0, 0}, {5, 0}), Circle({0, 0}, 1));
    ASSERT_EQ(pts.size(), 1u);
    EXPECT_TRUE(pts[0].approxEquals({1, 0}, 1e-9));
}

TEST(LineCircle, DiagonalSecant) {
    auto pts = intersectLineCircle(Line({-3, -3}, {3, 3}), Circle({0, 0}, std::sqrt(2.0)));
    ASSERT_EQ(pts.size(), 2u);
    for (Vec2 p : pts) EXPECT_NEAR(distance(p, {0, 0}), std::sqrt(2.0), 1e-9);
}

TEST(LineCircle, ZeroLengthSegment) {
    EXPECT_EQ(intersectLineCircle(Line({1, 0}, {1, 0}), Circle({0, 0}, 1)).size(), 1u);
    EXPECT_TRUE(intersectLineCircle(Line({2, 0}, {2, 0}), Circle({0, 0}, 1)).empty());
}

// ---- circle-circle ----
TEST(CircleCircle, TwoPointIntersection) {
    auto res = intersectCircleCircle(Circle({0, 0}, 1), Circle({1, 0}, 1));
    ASSERT_EQ(res.points.size(), 2u);
    EXPECT_FALSE(res.coincident);
    for (Vec2 p : res.points) {
        EXPECT_NEAR(distance(p, {0, 0}), 1.0, 1e-9);
        EXPECT_NEAR(distance(p, {1, 0}), 1.0, 1e-9);
    }
    EXPECT_NEAR(res.points[0].x, 0.5, 1e-9);
}

TEST(CircleCircle, ExternallyTangent) {
    auto res = intersectCircleCircle(Circle({0, 0}, 1), Circle({2, 0}, 1));
    ASSERT_EQ(res.points.size(), 1u);
    EXPECT_TRUE(res.points[0].approxEquals({1, 0}, 1e-9));
}

TEST(CircleCircle, InternallyTangent) {
    auto res = intersectCircleCircle(Circle({0, 0}, 3), Circle({1, 0}, 2));
    ASSERT_EQ(res.points.size(), 1u);
    EXPECT_TRUE(res.points[0].approxEquals({3, 0}, 1e-9));
    // Same configuration with the arguments swapped.
    auto sw = intersectCircleCircle(Circle({1, 0}, 2), Circle({0, 0}, 3));
    ASSERT_EQ(sw.points.size(), 1u);
    EXPECT_TRUE(sw.points[0].approxEquals({3, 0}, 1e-9));
}

TEST(CircleCircle, NearlyTangentWithinTolerance) {
    auto res = intersectCircleCircle(Circle({0, 0}, 1), Circle({2 + 1e-12, 0}, 1));
    EXPECT_EQ(res.points.size(), 1u);
}

TEST(CircleCircle, SeparateCircles) {
    EXPECT_TRUE(intersectCircleCircle(Circle({0, 0}, 1), Circle({5, 0}, 1)).points.empty());
}

TEST(CircleCircle, OneInsideTheOther) {
    auto res = intersectCircleCircle(Circle({0, 0}, 5), Circle({1, 0}, 1));
    EXPECT_TRUE(res.points.empty());
    EXPECT_FALSE(res.coincident);
}

TEST(CircleCircle, ConcentricDifferentRadii) {
    auto res = intersectCircleCircle(Circle({0, 0}, 1), Circle({0, 0}, 2));
    EXPECT_TRUE(res.points.empty());
    EXPECT_FALSE(res.coincident);
}

TEST(CircleCircle, IdenticalCirclesAreCoincident) {
    auto res = intersectCircleCircle(Circle({1, 1}, 2), Circle({1, 1}, 2));
    EXPECT_TRUE(res.coincident);
    EXPECT_TRUE(res.points.empty());
}

// ---- point in polygon ----
TEST(PointInPolygon, InsideOutsideBoundary) {
    std::vector<Vec2> sq{{0, 0}, {4, 0}, {4, 4}, {0, 4}};
    EXPECT_EQ(pointInPolygon({2, 2}, sq), PointLocation::Inside);
    EXPECT_EQ(pointInPolygon({5, 2}, sq), PointLocation::Outside);
    EXPECT_EQ(pointInPolygon({4, 2}, sq), PointLocation::Boundary);
    EXPECT_EQ(pointInPolygon({0, 0}, sq), PointLocation::Boundary);  // vertex
}

TEST(PointInPolygon, RayThroughVertexIsCountedOnce) {
    // Diamond: a horizontal ray from the left at y=0 passes exactly through two vertices.
    std::vector<Vec2> diamond{{0, -2}, {2, 0}, {0, 2}, {-2, 0}};
    EXPECT_EQ(pointInPolygon({-5, 0}, diamond), PointLocation::Outside);
    EXPECT_EQ(pointInPolygon({5, 0}, diamond), PointLocation::Outside);
    EXPECT_EQ(pointInPolygon({0, 0}, diamond), PointLocation::Inside);
}

TEST(PointInPolygon, ConcaveNotch) {
    std::vector<Vec2> u{{0, 0}, {5, 0}, {5, 5}, {4, 5}, {4, 1}, {1, 1}, {1, 5}, {0, 5}};  // U shape
    EXPECT_EQ(pointInPolygon({2.5, 3}, u), PointLocation::Outside);  // inside the cavity
    EXPECT_EQ(pointInPolygon({2.5, 0.5}, u), PointLocation::Inside);
    EXPECT_EQ(pointInPolygon({0.5, 3}, u), PointLocation::Inside);
}

TEST(PointInPolygon, ToleranceExpandsBoundary) {
    std::vector<Vec2> sq{{0, 0}, {4, 0}, {4, 4}, {0, 4}};
    EXPECT_EQ(pointInPolygon({4.05, 2}, sq), PointLocation::Outside);
    EXPECT_EQ(pointInPolygon({4.05, 2}, sq, 0.1), PointLocation::Boundary);
}

TEST(PointInPolygon, DegenerateInputs) {
    EXPECT_EQ(pointInPolygon({0, 0}, std::vector<Vec2>{}), PointLocation::Outside);
    std::vector<Vec2> line{{0, 0}, {2, 0}, {4, 0}};
    EXPECT_EQ(pointInPolygon({1, 0}, line), PointLocation::Boundary);
    EXPECT_EQ(pointInPolygon({1, 1}, line), PointLocation::Outside);
}

TEST(DistanceToSegment, ClampsToEndpoints) {
    EXPECT_DOUBLE_EQ(distanceToSegment({5, 0}, {0, 0}, {2, 0}), 3.0);
    EXPECT_DOUBLE_EQ(distanceToSegment({1, 2}, {0, 0}, {2, 0}), 2.0);
    EXPECT_DOUBLE_EQ(distanceToSegment({3, 4}, {0, 0}, {0, 0}), 5.0);
}
