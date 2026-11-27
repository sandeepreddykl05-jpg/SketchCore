#include "sketchcore/Scene.hpp"

#include <algorithm>

namespace sketchcore {

std::size_t Scene::indexOf(ShapeId id) const {
    const auto it = std::find_if(entries_.begin(), entries_.end(), [&](const Entry& e) { return e.id == id; });
    if (it == entries_.end()) throw std::out_of_range("Scene: unknown shape id");
    return static_cast<std::size_t>(it - entries_.begin());
}

ShapeId Scene::add(std::unique_ptr<Shape> shape) {
    if (!shape) throw std::invalid_argument("Scene::add: null shape");
    const ShapeId id = nextId_++;
    entries_.push_back({id, std::move(shape)});
    return id;
}

void Scene::insertAt(std::size_t index, ShapeId id, std::unique_ptr<Shape> shape) {
    if (!shape) throw std::invalid_argument("Scene::insertAt: null shape");
    if (index > entries_.size()) index = entries_.size();
    entries_.insert(entries_.begin() + static_cast<std::ptrdiff_t>(index), Entry{id, std::move(shape)});
    nextId_ = std::max(nextId_, id + 1);
}

std::unique_ptr<Shape> Scene::remove(ShapeId id, std::size_t* indexOut) {
    const std::size_t idx = indexOf(id);
    if (indexOut) *indexOut = idx;
    auto shape = std::move(entries_[idx].shape);
    entries_.erase(entries_.begin() + static_cast<std::ptrdiff_t>(idx));
    return shape;
}

std::unique_ptr<Shape> Scene::replace(ShapeId id, std::unique_ptr<Shape> shape) {
    if (!shape) throw std::invalid_argument("Scene::replace: null shape");
    auto& slot = entries_[indexOf(id)].shape;
    std::swap(slot, shape);
    return shape;
}

Shape* Scene::find(ShapeId id) {
    for (auto& e : entries_)
        if (e.id == id) return e.shape.get();
    return nullptr;
}

const Shape* Scene::find(ShapeId id) const {
    for (const auto& e : entries_)
        if (e.id == id) return e.shape.get();
    return nullptr;
}

std::optional<ShapeId> Scene::hitTest(Vec2 p, double tol) const {
    for (auto it = entries_.rbegin(); it != entries_.rend(); ++it)
        if (it->shape->contains(p, tol)) return it->id;
    return std::nullopt;
}

}  // namespace sketchcore
