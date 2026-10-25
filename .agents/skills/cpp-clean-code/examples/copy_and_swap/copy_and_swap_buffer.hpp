#pragma once

#include <algorithm>
#include <cstddef>
#include <new>
#include <utility>

/**
 * @file copy_and_swap_buffer.hpp
 * @brief Canonical Copy-and-Swap Idiom (Stephan Roth - Clean C++).
 *
 * Benefits:
 * 1. Strong Exception-Safety: Memory allocation happens during pass-by-value argument construction.
 *    If allocation throws, the target object remains untouched.
 * 2. Zero Code Duplication: Copy assignment reuses the copy constructor logic.
 * 3. Self-Assignment Safe: Handles 'a = a' seamlessly without manual 'if (this != &other)'.
 * 4. Move Assignment Optimization: Pass-by-value automatically serves both copy and move assignment!
 */

namespace patterns::memory {

class DynamicBuffer final {
public:
    // Default constructor
    constexpr DynamicBuffer() noexcept
        : m_data(nullptr)
        , m_size(0)
    {
    }

    // Allocation constructor
    explicit DynamicBuffer(std::size_t size)
        : m_data(size > 0 ? new char[size]() : nullptr)
        , m_size(size)
    {
    }

    // Destructor (RAII)
    ~DynamicBuffer() noexcept {
        delete[] m_data;
    }

    // 1. Copy Constructor: Allocates and copies data
    DynamicBuffer(const DynamicBuffer& other)
        : m_data(other.m_size > 0 ? new char[other.m_size] : nullptr)
        , m_size(other.m_size)
    {
        if (m_data && other.m_data) {
            std::copy(other.m_data, other.m_data + m_size, m_data);
        }
    }

    // 2. Move Constructor: Steals resources (noexcept)
    DynamicBuffer(DynamicBuffer&& other) noexcept
        : m_data(std::exchange(other.m_data, nullptr))
        , m_size(std::exchange(other.m_size, 0))
    {
    }

    // 3. COPY-AND-SWAP ASSIGNMENT OPERATOR:
    // Note: Parameter 'other' is passed BY VALUE!
    // - For lvalues: copy constructor is invoked to create 'other'.
    // - For rvalues: move constructor is invoked to create 'other'.
    DynamicBuffer& operator=(DynamicBuffer other) noexcept {
        swap(*this, other); // Swap internal pointers with local copy
        return *this;       // Old resource is automatically deleted when 'other' is destroyed
    }

    // Non-throwing swap function (ADL friendly)
    friend void swap(DynamicBuffer& lhs, DynamicBuffer& rhs) noexcept {
        using std::swap;
        swap(lhs.m_data, rhs.m_data);
        swap(lhs.m_size, rhs.m_size);
    }

    [[nodiscard]] std::size_t size() const noexcept { return m_size; }
    [[nodiscard]] char* data() noexcept { return m_data; }
    [[nodiscard]] const char* data() const noexcept { return m_data; }

private:
    char* m_data{nullptr};
    std::size_t m_size{0};
};

} // namespace patterns::memory
