#include <gtest/gtest.h>

#include "sketchcore/SketchCore.hpp"

using namespace sketchcore;

TEST(Vec2, ArithmeticDotCross) {
    const Vec2 a{3, 4}, b{1, 2};
    EXPECT_TRUE((a + b).approxEquals({4, 6}));
    EXPECT_TRUE((a - b).approxEquals({2, 2}));
    EXPECT_TRUE((a * 2).approxEquals({6, 8}));
    EXPECT_DOUBLE_EQ(a.dot(b), 11.0);
    EXPECT_DOUBLE_EQ(a.cross(b), 2.0);
    EXPECT_DOUBLE_EQ(a.length(), 5.0);
}

TEST(Vec2, NormalizedZeroVectorIsZeroNotNaN) {
    const Vec2 n = Vec2{0, 0}.normalized();
    EXPECT_EQ(n.x, 0.0);
    EXPECT_EQ(n.y, 0.0);
    EXPECT_TRUE(Vec2(0, 5).normalized().approxEquals({0, 1}));
}

TEST(Tolerance, RelativeForLargeAbsoluteForSmall) {
    EXPECT_TRUE(nearlyEqual(0.0, 1e-12));
    EXPECT_FALSE(nearlyEqual(0.0, 1e-6));
    EXPECT_TRUE(nearlyEqual(1e12, 1e12 + 1.0));  // relative
    EXPECT_FALSE(nearlyEqual(1.0, 1.001));
}

TEST(Matrix3, IdentityLeavesPointsUnchanged) {
    EXPECT_TRUE(Matrix3::identity().apply({7, -3}).approxEquals({7, -3}));
}

TEST(Matrix3, Translation) {
    EXPECT_TRUE(Matrix3::translation(2, -5).apply({1, 1}).approxEquals({3, -4}));
}

TEST(Matrix3, TranslationIgnoredForVectors) {
    EXPECT_TRUE(Matrix3::translation(2, -5).applyVector({1, 1}).approxEquals({1, 1}));
}

TEST(Matrix3, RotationQuarterTurn) {
    EXPECT_TRUE(Matrix3::rotation(kPi / 2).apply({1, 0}).approxEquals({0, 1}, 1e-12));
    EXPECT_TRUE(Matrix3::rotation(kPi).apply({1, 2}).approxEquals({-1, -2}, 1e-12));
}

TEST(Matrix3, RotationAboutPivot) {
    // (2,1) rotated 90 degrees CCW about (1,1) -> (1,2)
    EXPECT_TRUE(Matrix3::rotation(kPi / 2, {1, 1}).apply({2, 1}).approxEquals({1, 2}, 1e-12));
}

TEST(Matrix3, ScalingAboutPivotKeepsPivotFixed) {
    const auto m = Matrix3::scaling(3, 2, {1, 1});
    EXPECT_TRUE(m.apply({1, 1}).approxEquals({1, 1}));
    EXPECT_TRUE(m.apply({2, 2}).approxEquals({4, 3}));
}

TEST(Matrix3, CompositionAppliesRightmostFirst) {
    const auto scaleThenMove = Matrix3::translation(10, 0) * Matrix3::scaling(2, 2);
    EXPECT_TRUE(scaleThenMove.apply({1, 1}).approxEquals({12, 2}));
    const auto moveThenScale = Matrix3::scaling(2, 2) * Matrix3::translation(10, 0);
    EXPECT_TRUE(moveThenScale.apply({1, 1}).approxEquals({22, 2}));
}

TEST(Matrix3, Determinant) {
    EXPECT_DOUBLE_EQ(Matrix3::rotation(0.7).determinant(), 1.0);
    EXPECT_DOUBLE_EQ(Matrix3::scaling(2, 3).determinant(), 6.0);
    EXPECT_DOUBLE_EQ(Matrix3::scaling(2, 3).linearDeterminant(), 6.0);
}

TEST(Matrix3, InverseRoundTrip) {
    const Matrix3 m = Matrix3::translation(4, -2) * Matrix3::rotation(0.6) * Matrix3::scaling(2, 0.5);
    const auto inv = m.inverse();
    ASSERT_TRUE(inv.has_value());
    EXPECT_TRUE((m * *inv).approxEquals(Matrix3::identity(), 1e-12));
    const Vec2 p{3.3, -7.1};
    EXPECT_TRUE(inv->apply(m.apply(p)).approxEquals(p, 1e-12));
}

TEST(Matrix3, SingularMatrixHasNoInverse) {
    EXPECT_FALSE(Matrix3::scaling(0, 1).inverse().has_value());
}

TEST(BoundingBox, EmptyBehaviour) {
    BoundingBox b;
    EXPECT_TRUE(b.isEmpty());
    EXPECT_EQ(b.width(), 0.0);
    EXPECT_FALSE(b.contains({0, 0}));
    b.expand(Vec2{1, 1});
    EXPECT_FALSE(b.isEmpty());
}

TEST(BoundingBox, ExpandContainsIntersects) {
    BoundingBox a({0, 0}, {2, 2});
    BoundingBox b({1, 1}, {3, 3});
    BoundingBox c({5, 5}, {6, 6});
    EXPECT_TRUE(a.intersects(b));
    EXPECT_FALSE(a.intersects(c));
    EXPECT_TRUE(a.contains({2, 2}));
    EXPECT_FALSE(a.contains({2.1, 2}));
    EXPECT_TRUE(a.contains({2.1, 2}, 0.2));
    a.expand(b);
    EXPECT_DOUBLE_EQ(a.width(), 3.0);
}
