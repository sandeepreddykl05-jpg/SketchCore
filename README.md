# SketchCore

[![CI](https://github.com/sandeepreddykl05-jpg/SketchCore/actions/workflows/ci.yml/badge.svg)](https://github.com/sandeepreddykl05-jpg/SketchCore/actions/workflows/ci.yml)
![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![CMake](https://img.shields.io/badge/build-CMake-064F8C)
![Tests](https://img.shields.io/badge/unit%20tests-77-brightgreen)

SketchCore is a lightweight, UI-independent 2D geometry library written in modern C++17. It provides shapes, affine transforms, tolerance-aware intersection tests and a scene container with safe ownership. It was designed as the geometry backend for 2D sketching and CAD-style tools, and it knows nothing about any rendering or GUI framework.

It is used by **[SketchPad](https://github.com/sandeepreddykl05-jpg/SketchPad)**, a Qt desktop sketching app built on top of it.

## Key Features

### Math and primitives (`Vec2`, `Matrix3`, `BoundingBox`, `Tolerance`)
- Polymorphic `Shape` hierarchy: `Line` (segment), `Circle`, `Polygon` (concave polygons supported) and `Rectangle`.
- Every shape provides `area()`, `perimeter()`, `boundingBox()`, `contains(point, tol)` for hit-testing, `transform(Matrix3)` and `clone()`.
- `Rectangle` is stored as an origin plus two edge vectors (`u`, `v`), so any affine transform is applied exactly. Rotation keeps it a rectangle; shear turns it into a parallelogram.
- `Circle` scales its radius by the square root of the absolute determinant, so it stays a circle under similarity transforms. Under non-uniform scaling it is an approximation, since a true result would be an ellipse.
- `Matrix3` is a 3x3 homogeneous matrix with translate, rotate and scale (optionally about a pivot), composition, determinant and inverse.
- Floating-point comparisons go through one tolerance (`kEps`, `nearlyEqual`), and axis-aligned bounding boxes are included.

### Intersection and containment
- **Segment-segment** (`intersectLineLine`): crossing segments, parallel and nearly parallel lines, T-junctions, zero-length segments, and collinear overlap (returns the endpoints of the overlap).
- **Segment-circle** (`intersectLineCircle`): secant, tangent and miss cases, including segments that stop short of or start inside the circle.
- **Circle-circle** (`intersectCircleCircle`): two points, external and internal tangency, separate, nested, concentric and coincident circles.
- **Point-in-polygon** (`pointInPolygon`): ray casting with an explicit boundary test, returning `Inside`, `Boundary` or `Outside`. Works for concave and degenerate (collinear) polygons.
- All routines take a tolerance and convert it into parameter space, so long and short segments behave consistently.

### Scene container (`Scene`)
- Owns shapes through `std::unique_ptr`, with no raw `new` or `delete`. The scene is move-only.
- `emplace<T>(...)`, `add`, `remove`, `replace` and `insertAt` move ownership in and out explicitly, which makes undo/redo stacks simple and leak-free.
- Shape ids (`ShapeId`) increase monotonically and are never reused, so a stale id can't point at a different shape.
- Shapes are kept in z-order (last is topmost), and `hitTest(point, tol)` returns the topmost shape under a point, with a tolerance for click slop.

### Quality
- **Dependencies:** none at runtime. It builds as a static library.
- **Tests:** 77 GoogleTest unit tests covering edge cases such as parallel lines, tangent circles and degenerate polygons.
- **Memory safety:** checked with AddressSanitizer and UBSan on Linux in CI.
- **CI:** GitHub Actions builds with GCC, Clang, AppleClang and MSVC.

## Project Layout

```
SketchCore/
├── .github/workflows/ci.yml        # CI build and test pipeline
├── include/sketchcore/
│   ├── SketchCore.hpp              # Umbrella header
│   ├── Tolerance.hpp               # Floating-point comparison helpers
│   ├── Vec2.hpp                    # 2D vector algebra
│   ├── Matrix3.hpp                 # 2D affine transformation matrices
│   ├── BoundingBox.hpp             # Axis-aligned bounding boxes
│   ├── Shape.hpp                   # Abstract shape interface
│   ├── Shapes.hpp                  # Line, Circle, Rectangle, Polygon
│   ├── Intersections.hpp           # Intersection tests and point-in-polygon
│   └── Scene.hpp                   # Ownership, z-order and hit-testing
├── src/
│   ├── Shapes.cpp
│   ├── Intersections.cpp
│   └── Scene.cpp
├── tests/
│   ├── test_math.cpp
│   ├── test_shapes.cpp
│   ├── test_intersections.cpp
│   └── test_scene.cpp
└── CMakeLists.txt
```

## Building and Testing

**Requirements:** a C++17 compiler (GCC, Clang or MSVC) and CMake 3.16 or newer. GoogleTest is found on the system or downloaded automatically with CMake `FetchContent`.

**Linux, macOS, MinGW**
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

**Windows (MSVC)**
```bash
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

**With AddressSanitizer and UBSan** (GCC or Clang):
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DSKETCHCORE_ENABLE_ASAN=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Usage Example

Include the umbrella header and link against the `sketchcore` CMake target:

```cpp
#include <sketchcore/SketchCore.hpp>
#include <iostream>
#include <vector>

int main() {
    using namespace sketchcore;

    // 1. Create a scene; shapes are owned by the scene and stacked in insertion order
    Scene scene;
    ShapeId lineId = scene.emplace<Line>(Vec2{0.0, 0.0}, Vec2{10.0, 10.0});
    ShapeId circleId = scene.emplace<Circle>(Vec2{5.0, 5.0}, 3.0);
    std::cout << "Created shapes " << lineId << " and " << circleId << "\n";

    // 2. Query the topmost shape at a cursor position (tolerance widens the hit area)
    if (auto hit = scene.hitTest(Vec2{5.0, 5.0}, 0.5)) {
        std::cout << "Hit topmost shape ID: " << *hit << "\n";
    }

    // 3. Intersect two segments
    Line segA{Vec2{0.0, 0.0}, Vec2{10.0, 10.0}};
    Line segB{Vec2{0.0, 10.0}, Vec2{10.0, 0.0}};
    std::vector<Vec2> points = intersectLineLine(segA, segB);

    std::cout << "Intersection count: " << points.size() << "\n";
    if (!points.empty()) std::cout << "Point: (" << points[0].x << ", " << points[0].y << ")\n";
    return 0;
}
```

Output:
```
Created shapes 1 and 2
Hit topmost shape ID: 2
Intersection count: 1
Point: (5, 5)
```

## Design Notes

- **Tolerance, not exactness.** Floating-point results are compared with a tolerance. Intersection routines treat nearly parallel lines as parallel and nearly tangent circles as tangent, instead of returning far-away or spurious points.
- **Parallel and collinear lines.** Parallel lines have no intersection. Collinear overlapping segments return the two endpoints of the overlap.
- **Ownership.** `remove()` hands the `unique_ptr` back to the caller. That is what lets an application keep removed shapes inside undo commands without copies or leaks.

## Roadmap

- [ ] Spatial index (grid or quadtree) to speed up hit-testing on large scenes
- [ ] Arc and Bezier curve primitives
- [ ] Segment-polygon and polygon-polygon intersection
