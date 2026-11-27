#include <gtest/gtest.h>

#include "sketchcore/SketchCore.hpp"

using namespace sketchcore;

TEST(Scene, EmplaceAssignsUniqueIdsAndStoresShapes) {
    Scene s;
    const ShapeId a = s.emplace<Circle>(Vec2{0, 0}, 1.0);
    const ShapeId b = s.emplace<Line>(Vec2{0, 0}, Vec2{1, 1});
    EXPECT_NE(a, b);
    EXPECT_EQ(s.size(), 2u);
    EXPECT_TRUE(s.findAs<Circle>(a) != nullptr);
    EXPECT_TRUE(s.findAs<Circle>(b) == nullptr);  // wrong dynamic type
    EXPECT_TRUE(s.find(9999) == nullptr);
}

TEST(Scene, AddNullThrows) {
    Scene s;
    EXPECT_THROW(s.add(nullptr), std::invalid_argument);
}

TEST(Scene, HitTestReturnsTopmost) {
    Scene s;
    const ShapeId bottom = s.emplace<Circle>(Vec2{0, 0}, 5.0);
    const ShapeId top = s.emplace<Circle>(Vec2{0, 0}, 1.0);
    EXPECT_EQ(*s.hitTest({0, 0}), top);
    EXPECT_EQ(*s.hitTest({3, 0}), bottom);
    EXPECT_FALSE(s.hitTest({10, 0}).has_value());
}

TEST(Scene, RemoveReleasesOwnershipAndInsertAtRestoresZOrder) {
    Scene s;
    const ShapeId a = s.emplace<Circle>(Vec2{0, 0}, 1.0);
    const ShapeId b = s.emplace<Circle>(Vec2{5, 0}, 1.0);
    const ShapeId c = s.emplace<Circle>(Vec2{10, 0}, 1.0);
    std::size_t index = 99;
    auto taken = s.remove(b, &index);
    ASSERT_TRUE(taken != nullptr);
    EXPECT_EQ(index, 1u);
    EXPECT_EQ(s.size(), 2u);
    s.insertAt(index, b, std::move(taken));
    ASSERT_EQ(s.size(), 3u);
    EXPECT_EQ(s.entries()[0].id, a);
    EXPECT_EQ(s.entries()[1].id, b);
    EXPECT_EQ(s.entries()[2].id, c);
}

TEST(Scene, RemoveUnknownIdThrows) {
    Scene s;
    EXPECT_THROW(s.remove(42), std::out_of_range);
}

TEST(Scene, ReplaceKeepsIdAndReturnsOld) {
    Scene s;
    const ShapeId a = s.emplace<Circle>(Vec2{0, 0}, 1.0);
    auto old = s.replace(a, std::make_unique<Circle>(Vec2{0, 0}, 9.0));
    EXPECT_NEAR(static_cast<Circle*>(old.get())->radius(), 1.0, 1e-12);
    EXPECT_NEAR(s.findAs<Circle>(a)->radius(), 9.0, 1e-12);
}

TEST(Scene, IdsAreNotReusedAfterRemoval) {
    Scene s;
    const ShapeId a = s.emplace<Circle>(Vec2{0, 0}, 1.0);
    s.remove(a);
    const ShapeId b = s.emplace<Circle>(Vec2{0, 0}, 1.0);
    EXPECT_NE(a, b);
}

TEST(Scene, IsMoveOnlyAndMovePreservesContents) {
    static_assert(!std::is_copy_constructible_v<Scene>, "Scene must not be copyable");
    static_assert(std::is_nothrow_move_constructible_v<Scene>, "Scene move must be noexcept");
    Scene s;
    s.emplace<Line>(Vec2{0, 0}, Vec2{1, 0});
    Scene t = std::move(s);
    EXPECT_EQ(t.size(), 1u);
}
