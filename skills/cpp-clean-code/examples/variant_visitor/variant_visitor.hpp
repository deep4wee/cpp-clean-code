#pragma once

#include <cmath>
#include <iostream>
#include <variant>
#include <vector>

/**
 * @file variant_visitor.hpp
 * @brief Modern C++ Visitor via std::variant and the Overloaded Pattern (Klaus Iglberger - Guideline 17).
 *
 * Solves the Expression Problem for: Closed Set of Types, Open Set of Operations.
 *
 * Advantages over Classic GoF Cyclic Visitor:
 * 1. Zero Virtual Tables: No virtual functions, no double-dispatch overhead.
 * 2. Zero Class Invasiveness: Shape classes do NOT require an accept(Visitor&) method.
 * 3. Cache Locality: std::variant elements stored inline within std::vector, avoiding heap fragmentation.
 * 4. Extensibility: New operations (perimeter, area, serialization) added as standalone visitors/functions.
 */

namespace patterns::visitor {

// 1. Plain Data Types (Zero inheritance, pure value semantics)
struct Circle {
    double radius{1.0};
};

struct Rectangle {
    double width{2.0};
    double height{3.0};
};

struct Triangle {
    double base{3.0};
    double height{4.0};
};

// 2. Closed polymorphic set of types
using Shape = std::variant<Circle, Rectangle, Triangle>;

// 3. The Overloaded helper idiom (variadic template inheritance)
template <typename... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

// Explicit deduction guide (needed for C++17, implicit in C++20)
template <typename... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

// 4. Open Operations implemented as non-member functions:

// Operation A: Calculate Area
[[nodiscard]] inline double area(const Shape& shape) {
    return std::visit(
        Overloaded{
            [](const Circle& c) { return 3.1415926535 * c.radius * c.radius; },
            [](const Rectangle& r) { return r.width * r.height; },
            [](const Triangle& t) { return 0.5 * t.base * t.height; }
        },
        shape
    );
}

// Operation B: Render / Print Shape
inline void render(const Shape& shape) {
    std::visit(
        Overloaded{
            [](const Circle& c) { std::cout << "Rendering Circle(r=" << c.radius << ")\n"; },
            [](const Rectangle& r) { std::cout << "Rendering Rectangle(" << r.width << "x" << r.height << ")\n"; },
            [](const Triangle& t) { std::cout << "Rendering Triangle(b=" << t.base << ", h=" << t.height << ")\n"; }
        },
        shape
    );
}

// Verification usage:
inline void demo_variant_visitor() {
    std::vector<Shape> shapes;
    shapes.emplace_back(Circle{5.0});
    shapes.emplace_back(Rectangle{4.0, 6.0});
    shapes.emplace_back(Triangle{3.0, 8.0});

    for (const auto& shape : shapes) {
        render(shape);
        std::cout << " -> Area: " << area(shape) << '\n';
    }
}

} // namespace patterns::visitor
