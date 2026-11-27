#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "sketchcore/Shape.hpp"

namespace sketchcore {

using ShapeId = std::uint64_t;

// Owns a z-ordered collection of shapes (last = topmost) through unique_ptr.
// There is no raw new/delete anywhere; ownership moves explicitly in and out.
class Scene {
public:
    struct Entry {
        ShapeId id;
        std::unique_ptr<Shape> shape;
    };

    Scene() = default;
    Scene(Scene&&) noexcept = default;
    Scene& operator=(Scene&&) noexcept = default;
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    // Constructs a shape of type T in place. Template keeps call sites type-safe:
    //   scene.emplace<Circle>(Vec2{0, 0}, 5.0);
    template <class T, class... Args>
    ShapeId emplace(Args&&... args) {
        static_assert(std::is_base_of_v<Shape, T>, "T must derive from Shape");
        return add(std::make_unique<T>(std::forward<Args>(args)...));
    }

    // Takes ownership; throws std::invalid_argument on nullptr.
    ShapeId add(std::unique_ptr<Shape> shape);
    // Re-inserts a previously removed shape with its original id and z-position.
    void insertAt(std::size_t index, ShapeId id, std::unique_ptr<Shape> shape);
    // Releases ownership back to the caller; throws std::out_of_range for unknown ids.
    std::unique_ptr<Shape> remove(ShapeId id, std::size_t* indexOut = nullptr);
    // Swaps in a new shape under the same id and returns the old one.
    std::unique_ptr<Shape> replace(ShapeId id, std::unique_ptr<Shape> shape);

    Shape* find(ShapeId id);
    const Shape* find(ShapeId id) const;
    template <class T>
    T* findAs(ShapeId id) { return dynamic_cast<T*>(find(id)); }

    // Topmost shape hit by p (tol widens the hit area, e.g. 6px / zoom).
    std::optional<ShapeId> hitTest(Vec2 p, double tol = kEps) const;

    std::size_t size() const { return entries_.size(); }
    bool empty() const { return entries_.empty(); }
    void clear() { entries_.clear(); }
    const std::vector<Entry>& entries() const { return entries_; }

private:
    std::size_t indexOf(ShapeId id) const;

    std::vector<Entry> entries_;
    ShapeId nextId_ = 1;
};

}  // namespace sketchcore
