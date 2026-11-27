cmake_minimum_required(VERSION 3.16)
project(SketchCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SKETCHCORE_BUILD_TESTS "Build the unit tests" ON)
option(SKETCHCORE_ENABLE_ASAN "Build with AddressSanitizer + UBSan" OFF)

if(SKETCHCORE_ENABLE_ASAN AND NOT MSVC)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

add_library(sketchcore
  src/Shapes.cpp
  src/Intersections.cpp
  src/Scene.cpp)
add_library(SketchCore::sketchcore ALIAS sketchcore)

target_include_directories(sketchcore PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(sketchcore PUBLIC cxx_std_17)

if(MSVC)
  target_compile_options(sketchcore PRIVATE /W4)
else()
  target_compile_options(sketchcore PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

if(SKETCHCORE_BUILD_TESTS)
  enable_testing()
  find_package(GTest QUIET)
  if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
  endif()

  add_executable(sketchcore_tests
    tests/test_math.cpp
    tests/test_shapes.cpp
    tests/test_intersections.cpp
    tests/test_scene.cpp)
  target_link_libraries(sketchcore_tests PRIVATE sketchcore GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(sketchcore_tests)
endif()
