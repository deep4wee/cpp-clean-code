#pragma once

#include <iostream>
#include <memory>
#include <utility>

/**
 * @file any_shape.hpp
 * @brief Modern C++ Type Erasure Pattern (Klaus Iglberger - C++ Software Design, Guidelines 32-34).
 *
 * Core Concept:
 * A compound design pattern consisting of:
 * 1. External Polymorphism (non-intrusive wrapper around unrelated types)
 * 2. Bridge (internal opaque pointer to conceptual interface)
 * 3. Prototype (virtual clone method providing Value Semantics)
 *
 * Key Benefits:
 * - Value Semantics: Behaves like a regular value (copyable, movable, deep const).
 * - Non-Intrusive: Types wrapped (Circle, Square) do NOT inherit from any common base class.
 * - Cache Friendly: Can be stored in a contiguous std::vector<AnyShape>.
 */

namespace patterns::type_erasure {

class AnyShape final {
private:
    // 1. External Polymorphism: Abstract concept interface
    struct ShapeConcept {
        virtual ~ShapeConcept() = default;
        virtual void draw() const = 0;
        [[nodiscard]] virtual std::unique_ptr<ShapeConcept> clone() const = 0; // Prototype
    };

    // 2. Concrete Model templated on ShapeT
    template <typename ShapeT>
    struct ShapeModel final : public ShapeConcept {
        explicit ShapeModel(ShapeT value) : m_value(std::move(value)) {}

        void draw() const override {
            // Uses ADL (Argument Dependent Lookup) or member method
            draw_shape(m_value);
        }

        [[nodiscard]] std::unique_ptr<ShapeConcept> clone() const override {
            return std::make_unique<ShapeModel<ShapeT>>(*this);
        }

        ShapeT m_value;
    };

    // 3. Bridge: Pointer to abstract concept
    std::unique_ptr<ShapeConcept> m_pimpl;

public:
    // Templated Constructor: Erases the concrete type!
    template <
        typename ShapeT,
        typename = std::enable_if_t<!std::is_same_v<std::decay_t<ShapeT>, AnyShape>>
    >
    AnyShape(ShapeT&& value)
        : m_pimpl(std::make_unique<ShapeModel<std::decay_t<ShapeT>>>(std::forward<ShapeT>(value)))
    {
    }

    // Value Semantics: Copy constructor via Prototype clone()
    AnyShape(const AnyShape& other)
        : m_pimpl(other.m_pimpl ? other.m_pimpl->clone() : nullptr)
    {
    }

    // Copy-and-Swap assignment operator
    AnyShape& operator=(AnyShape other) noexcept {
        m_pimpl.swap(other.m_pimpl);
        return *this;
    }

    // Move semantics (Rule of Five)
    AnyShape(AnyShape&&) noexcept = default;
    AnyShape& operator=(AnyShape&&) noexcept = default;
    ~AnyShape() = default;

    // Interface: Exposed via Hidden Friend (ADL)
    friend void draw(const AnyShape& shape) {
        if (shape.m_pimpl) {
            shape.m_pimpl->draw();
        }
    }
};

// -------------------------------------------------------------
// Demonstration of completely non-intrusive client types:
// Note: Neither Circle nor Square inherit from AnyShape!
// -------------------------------------------------------------
struct Circle {
    double radius{1.0};
};

inline void draw_shape(const Circle& c) {
    std::cout << "Drawing Circle with radius: " << c.radius << '\n';
}

struct Square {
    double side{2.0};
};

inline void draw_shape(const Square& s) {
    std::cout << "Drawing Square with side: " << s.side << '\n';
}

} // namespace patterns::type_erasure
