#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

/**
 * @file result.hpp
 * @brief Modern Monadic Error Handling (Inspired by C++23 std::expected and Modern C++ Specialist).
 *
 * Replaces:
 * 1. C-style out-parameters (User& out_user).
 * 2. Heavy exceptions for regular business logic / expected failures.
 * 3. std::optional (which loses the reason why the operation failed).
 *
 * Provides Monadic Operators:
 * - and_then: Chains operations that return another Result.
 * - transform: Maps the success value to a new value.
 * - value_or: Provides a default fallback value.
 */

namespace patterns::error_handling {

template <typename ErrorType>
struct Unexpected {
    ErrorType error;
    explicit Unexpected(ErrorType err) : error(std::move(err)) {}
};

template <typename T, typename E>
class Result final {
private:
    std::variant<T, E> m_storage;

public:
    // Success constructor
    Result(T value) : m_storage(std::in_place_index<0>, std::move(value)) {}

    // Error constructor
    Result(Unexpected<E> unexpected) : m_storage(std::in_place_index<1>, std::move(unexpected.error)) {}

    [[nodiscard]] bool has_value() const noexcept {
        return m_storage.index() == 0;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return has_value();
    }

    [[nodiscard]] const T& value() const {
        if (!has_value()) {
            throw std::logic_error("Attempted to access value on error Result");
        }
        return std::get<0>(m_storage);
    }

    [[nodiscard]] const E& error() const {
        if (has_value()) {
            throw std::logic_error("Attempted to access error on success Result");
        }
        return std::get<1>(m_storage);
    }

    [[nodiscard]] T value_or(T fallback) const {
        return has_value() ? std::get<0>(m_storage) : std::move(fallback);
    }

    // Monadic transform: Maps Result<T, E> -> Result<U, E> via Func(T) -> U
    template <typename Func>
    auto transform(Func&& func) const -> Result<decltype(func(std::declval<T>())), E> {
        using ReturnType = decltype(func(std::declval<T>()));
        if (has_value()) {
            return Result<ReturnType, E>(func(std::get<0>(m_storage)));
        }
        return Result<ReturnType, E>(Unexpected<E>(std::get<1>(m_storage)));
    }

    // Monadic and_then: Chains Result<T, E> -> Result<U, E> via Func(T) -> Result<U, E>
    template <typename Func>
    auto and_then(Func&& func) const -> decltype(func(std::declval<T>())) {
        if (has_value()) {
            return func(std::get<0>(m_storage));
        }
        using ResultType = decltype(func(std::declval<T>()));
        return ResultType(Unexpected<E>(std::get<1>(m_storage)));
    }
};

// Example domain usage:
struct User {
    int id;
    std::string name;
    int age;
};

enum class DomainError {
    UserNotFound,
    AccessDenied,
    InvalidInput
};

inline Result<User, DomainError> fetch_user_by_id(int id) {
    if (id <= 0) {
        return Unexpected(DomainError::InvalidInput);
    }
    if (id == 42) {
        return User{42, "Arthur Dent", 30};
    }
    return Unexpected(DomainError::UserNotFound);
}

inline Result<std::string, DomainError> verify_adult(const User& user) {
    if (user.age < 18) {
        return Unexpected(DomainError::AccessDenied);
    }
    return user.name + " [Verified Adult]";
}

// Fluent Monadic Pipeline Demonstration:
inline void demo_monadic_pipeline() {
    auto outcome = fetch_user_by_id(42)
        .and_then(verify_adult)
        .transform([](const std::string& verified_name) {
            return "SUCCESS: " + verified_name;
        });

    if (outcome) {
        std::cout << outcome.value() << '\n';
    } else {
        std::cout << "Operation failed with error code: " << static_cast<int>(outcome.error()) << '\n';
    }
}

} // namespace patterns::error_handling
