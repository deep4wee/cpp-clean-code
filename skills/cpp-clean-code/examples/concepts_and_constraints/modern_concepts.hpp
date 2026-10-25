#pragma once

#include <concepts>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

/**
 * @file modern_concepts.hpp
 * @brief C++20 Concepts and Constraints (Bjarne Stroustrup - A Tour of C++ / Core Guidelines T.10-T.13).
 *
 * Why Concepts completely supersede C++11/14 SFINAE & std::enable_if_t:
 * 1. Readability: Constraints expressed directly at function declaration.
 * 2. Diagnostics: Compilation errors pinpoint the exact unsatisfied requirement instead of pages of template trace.
 * 3. Overload Resolution: Compilers automatically rank functions by constraint subsumption (more constrained wins).
 * 4. Terse Syntax: 'auto' with concept names allows concise, type-safe generic functions.
 */

namespace patterns::concepts {

// 1. Defining a Custom Concept using requires-expressions
template <typename T>
concept Printable = requires(std::ostream& os, const T& item) {
    { os << item } -> std::same_as<std::ostream&>;
};

template <typename T>
concept Serializable = requires(T obj) {
    { obj.serialize() } -> std::convertible_to<std::string>;
};

// Compound Concept: Both Printable and EqualityComparable
template <typename T>
concept Auditable = Printable<T> && std::equality_comparable<T>;

// 2. Applying Concepts to Functions

// A. Terse syntax (recommended for simple constraints)
inline void log_item(const Printable auto& item) {
    std::cout << "[LOG]: " << item << '\n';
}

// B. Template header constraint
template <Auditable T>
inline bool are_audit_records_equal(const T& a, const T& b) {
    log_item(a);
    log_item(b);
    return a == b;
}

// C. Concept-Based Function Overloading (Replacing tag-dispatch and SFINAE)
template <typename T>
requires std::integral<T>
void process_value(T val) {
    std::cout << "Optimized integer path: " << (val * 2) << '\n';
}

template <typename T>
requires std::floating_point<T>
void process_value(T val) {
    std::cout << "Optimized floating point path: " << (val / 2.0) << '\n';
}

// 3. Applying Concepts to Classes & Constructors
template <Printable DataT>
class AuditRecord final {
public:
    explicit AuditRecord(DataT data) : m_data(std::move(data)) {}

    void print() const {
        std::cout << "Record: " << m_data << '\n';
    }

private:
    DataT m_data;
};

} // namespace patterns::concepts
