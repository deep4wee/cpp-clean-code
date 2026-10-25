#pragma once

#include <iostream>
#include <utility>

/**
 * @file strong_types.hpp
 * @brief Type-Rich Domain Modeling & Physical Units (Stephan Roth - Clean C++).
 *
 * Core Tenet: "Don't trust names. Trust types. Types don't lie."
 * Eliminates Primitive Obsession, prevents swapping parameters of same underlying type,
 * and enforces dimensional correctness at compile-time with ZERO runtime overhead.
 */

namespace domain::units {

// Generic StrongType wrapper using phantom tag types
template <typename ValueType, typename Tag>
class StrongType final {
public:
    using value_type = ValueType;

    constexpr explicit StrongType(ValueType val) : m_value(std::move(val)) {}

    [[nodiscard]] constexpr const ValueType& get() const noexcept { return m_value; }
    [[nodiscard]] constexpr ValueType& get() noexcept { return m_value; }

    constexpr explicit operator ValueType() const noexcept { return m_value; }

    // Arithmetic operators preserving type
    constexpr StrongType& operator+=(const StrongType& other) noexcept {
        m_value += other.m_value;
        return *this;
    }

    constexpr StrongType& operator-=(const StrongType& other) noexcept {
        m_value -= other.m_value;
        return *this;
    }

    friend constexpr StrongType operator+(StrongType lhs, const StrongType& rhs) noexcept {
        lhs += rhs;
        return lhs;
    }

    friend constexpr StrongType operator-(StrongType lhs, const StrongType& rhs) noexcept {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr bool operator==(const StrongType& lhs, const StrongType& rhs) noexcept {
        return lhs.m_value == rhs.m_value;
    }

    friend constexpr bool operator!=(const StrongType& lhs, const StrongType& rhs) noexcept {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(const StrongType& lhs, const StrongType& rhs) noexcept {
        return lhs.m_value < rhs.m_value;
    }

private:
    ValueType m_value;
};

// Distinct Domain Tags
struct MeterTag {};
struct SecondTag {};
struct NewtonTag {};
struct KilogramTag {};
struct VelocityTag {};

// Strongly Typed Aliases
using Meter = StrongType<double, MeterTag>;
using Second = StrongType<double, SecondTag>;
using Newton = StrongType<double, NewtonTag>;
using Kilogram = StrongType<double, KilogramTag>;
using Velocity = StrongType<double, VelocityTag>;

// Dimensional Arithmetic: Distance / Time = Velocity
[[nodiscard]] constexpr Velocity operator/(Meter distance, Second time) noexcept {
    return Velocity(distance.get() / time.get());
}

// User-Defined Literals (UDLs) for clean, self-documenting syntax
namespace literals {

constexpr Meter operator"" _m(long double val) noexcept {
    return Meter(static_cast<double>(val));
}

constexpr Meter operator"" _m(unsigned long long val) noexcept {
    return Meter(static_cast<double>(val));
}

constexpr Second operator"" _s(long double val) noexcept {
    return Second(static_cast<double>(val));
}

constexpr Second operator"" _s(unsigned long long val) noexcept {
    return Second(static_cast<double>(val));
}

constexpr Newton operator"" _N(long double val) noexcept {
    return Newton(static_cast<double>(val));
}

constexpr Kilogram operator"" _kg(long double val) noexcept {
    return Kilogram(static_cast<double>(val));
}

} // namespace literals

// Example domain function completely protected against parameter swapping:
inline void launch_projectile(Meter target_distance, Velocity initial_velocity) {
    std::cout << "Launching projectile to target at " << target_distance.get() 
              << " meters with velocity " << initial_velocity.get() << " m/s\n";
}

} // namespace domain::units
