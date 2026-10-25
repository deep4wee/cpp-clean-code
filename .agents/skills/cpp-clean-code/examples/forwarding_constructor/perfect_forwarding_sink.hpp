#pragma once

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

/**
 * @file perfect_forwarding_sink.hpp
 * @brief Constrained Universal Reference Constructor (Scott Meyers - Items 26 & 27).
 *
 * Problem:
 * An unconstrained universal reference constructor `template<typename T> Person(T&&)`
 * is greedy. For non-const lvalues (`Person p1("Alice"); Person p2(p1);`),
 * the template constructor is a closer match than `Person(const Person&)`!
 *
 * Solution:
 * Constrain the template constructor via std::enable_if_t (SFINAE) to ensure:
 * 1. It is disabled if T is Person or derived from Person.
 * 2. It is disabled if T is an integer or incompatible type.
 * 3. It perfectly forwards strings and string literals without temporary allocations.
 */

namespace patterns::metaprogramming {

class Person final {
public:
    // SFINAE-constrained Universal Reference Constructor
    template <
        typename T,
        typename = std::enable_if_t<
            !std::is_same_v<std::decay_t<T>, Person> &&
            !std::is_base_of_v<Person, std::decay_t<T>> &&
            std::is_constructible_v<std::string, T>
        >
    >
    explicit Person(T&& name)
        : m_name(std::forward<T>(name))
    {
    }

    // Explicit integer ID overload to demonstrate overload disambiguation
    explicit Person(int id)
        : m_name("ID_" + std::to_string(id))
    {
    }

    // Defaulted Rule of Five: Copy & Move constructors are preserved and work properly!
    Person(const Person&) = default;
    Person& operator=(const Person&) = default;
    Person(Person&&) noexcept = default;
    Person& operator=(Person&&) noexcept = default;
    ~Person() = default;

    [[nodiscard]] const std::string& name() const noexcept {
        return m_name;
    }

private:
    std::string m_name;
};

// Verification usage:
inline void demo_person_forwarding() {
    std::string name = "Eleanor";
    
    // 1. Forwards lvalue std::string -> calls Person(T&&) -> std::string copy constructor
    Person p1(name);

    // 2. Forwards rvalue std::string -> calls Person(T&&) -> std::string move constructor
    Person p2(std::string("Chidi"));

    // 3. Forwards string literal -> calls Person(T&&) -> constructs string in-place
    Person p3("Tahani");

    // 4. Copy constructor is called correctly! Without SFINAE, this would fail to compile!
    Person p4(p1);

    // 5. Move constructor is called correctly!
    Person p5(std::move(p2));
}

} // namespace patterns::metaprogramming
