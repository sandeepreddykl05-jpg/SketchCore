# SketchCore

[![CI](https://github.com)](https://github.com)
![C++](https://shields.io)
![CMake](https://shields.io)
![Tests](https://shields.io)

**SketchCore** is a lightweight, UI-independent 2D geometry, intersection, and scene-management engine written in modern C++. Designed as the core mathematical and state backend for 2D CAD tools, vector graphics editors, and spatial collision systems, it decouples geometric computation and object lifecycle management from any specific rendering framework.

---

## Key Features

- **2D Math & Spatial Primitives (`Vec2`, `Matrix3`, `BoundingBox`, `Tolerance`):**
  - Polymorphic `Shape` hierarchy supporting `Line` segments, `Circle`, `Polygon` (including concave polygons), and affine-closed `Rectangle` primitives (`origin + u, v` basis representation that preserves exact geometry under rotation, shear, and non-uniform scaling).
  - Configurable floating-point epsilon comparisons (`kEps`) and axis-aligned bounding boxes (AABB).
- **Robust Geometric Intersection & Containment Solvers:**
  - **Segment–Segment (`intersectLineLine`):** Handles crossing segments, parallel/nearly parallel lines, T-junctions, zero-length degenerate segments, and collinear overlapping intervals.
  - **Segment–Circle (`intersectLineCircle`) & Circle–Circle (`intersectCircleCircle`):** Computes exact secant, tangent, and coincident circle states.
  - **Point-in-Polygon (`pointInPolygon`):** Ray-casting algorithm with explicit boundary tolerance (`Inside`, `Boundary`, `Outside`) supporting concave and degenerate polygons.
- **Move-Only Scene Graph & Z-Order Management (`Scene`):**
  - Strict `std::unique_ptr` ownership transfer (`emplace`, `add`, `remove`, `replace`, `insertAt`) with zero raw `new`/`delete`, making Undo/Redo command stacks clean and memory-safe.
  - Monotonically increasing, non-reusable `ShapeId` handles to prevent stale reference bugs.
  - Layer-aware Z-order tracking and zoom-tolerant coordinate hit-testing (`hitTest(p, tol)`).
- **Zero External Runtime Dependencies:** Compiles as a clean static library (`libsketchcore.a`) ready to embed into desktop UIs (Raylib, Qt, ImGui), game engines, or WebAssembly frontends.
- **Automated Verification:** Backed by **77 unit tests** using GoogleTest and continuous integration via GitHub Actions.

---

## Architecture & Project Layout

SketchCore is organized into three decoupled layers—**Math & Primitives**, **Intersection Engine**, and **Scene Management**—with public headers cleanly separated from implementation files:

```text
SketchCore/
├── .github/workflows/ci.yml        # Automated CI build & test pipeline
├── include/sketchcore/
│   ├── SketchCore.hpp              # Single-include umbrella header
│   ├── Tolerance.hpp               # Floating-point epsilon utilities
│   ├── Vec2.hpp                    # 2D vector algebra
│   ├── Matrix3.hpp                 # 2D affine transformation matrices
│   ├── BoundingBox.hpp             # Axis-aligned bounding boxes (AABB)
│   ├── Shape.hpp                   # Base polymorphic shape interface
│   ├── Shapes.hpp                  # Line, Circle, Rectangle, & Polygon primitives
│   ├── Intersections.hpp           # Line/Circle intersections & point-in-polygon
│   └── Scene.hpp                   # Ownership, Z-order, & hit-testing container
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

---

## Building & Running Tests

### Prerequisites
- **C++17** (or newer) compatible compiler (GCC, Clang, or MSVC)
- **CMake** 3.16+ *(GoogleTest is fetched automatically via CMake `FetchContent`)*

### Build & Test (Linux / macOS / MinGW)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

### Build & Test (Windows MSVC)
```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

---

## Usage Example

Include the umbrella header `<sketchcore/SketchCore.hpp>` and link against the `sketchcore` CMake target:

```cpp
#include <sketchcore/SketchCore.hpp>
#include <iostream>

int main() {
    using namespace sketchcore;

    // 1. Create a scene and emplace shapes with automatic Z-order tracking
    Scene scene;
    ShapeId lineId = scene.emplace<Line>(Vec2{0.0, 0.0}, Vec2{10.0, 10.0});
    ShapeId circleId = scene.emplace<Circle>(Vec2{5.0, 5.0}, 3.0);

    // 2. Query the topmost shape at a cursor coordinate (with hit tolerance)
    if (auto hit = scene.hitTest(Vec2{5.0, 5.0}, 0.5)) {
        std::cout << "Hit topmost shape ID: " << *hit << "\n";
    }

    // 3. Compute exact geometric intersections
    Line segA{Vec2{0.0, 0.0}, Vec2{10.0, 10.0}};
    Line segB{Vec2{0.0, 10.0}, Vec2{10.0, 0.0}};
    std::vector<Vec2> points = intersectLineLine(segA, segB);

    std::cout << "Intersection count: " << points.size() << "\n";
    return 0;
}
```

---

## Roadmap

- [ ] Spatial indexing (Quadtree / R-tree) for large-scene hit-testing
- [ ] Arc and Bezier curve primitives with bounding-box acceleration
- [ ] Interactive visual sandbox frontend using Raylib / Dear ImGui
