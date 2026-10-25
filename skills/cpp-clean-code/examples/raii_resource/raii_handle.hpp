#pragma once

#include <utility>
#include <cstdio>

/**
 * @file raii_handle.hpp
 * @brief Generic RAII Wrapper for Raw OS & C Resources (Handles, Descriptors, Pointers).
 *
 * Demonstrates:
 * 1. Strict RAII: Acquire in constructor / factory, automatically release in destructor.
 * 2. Rule of Five: Move-only semantics (copies deleted to prevent double-free).
 * 3. Exception Safety: No resource leaks on thrown exceptions.
 * 4. Zero-Cost Abstraction: Compiles down to simple handle and cleanup calls.
 */

namespace patterns::raii {

// Policy-based traits for resource management
template <typename HandleType, HandleType InvalidValue, typename DeleterFunc>
class UniqueHandle final {
public:
    constexpr UniqueHandle() noexcept : m_handle(InvalidValue) {}

    explicit UniqueHandle(HandleType handle) noexcept : m_handle(handle) {}

    ~UniqueHandle() noexcept {
        reset();
    }

    // Move-only: Disallow copying to prevent multiple ownership
    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;

    // Move Constructor: Transfers ownership
    UniqueHandle(UniqueHandle&& other) noexcept : m_handle(other.release()) {}

    // Move Assignment
    UniqueHandle& operator=(UniqueHandle&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    [[nodiscard]] bool is_valid() const noexcept {
        return m_handle != InvalidValue;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return is_valid();
    }

    [[nodiscard]] HandleType get() const noexcept {
        return m_handle;
    }

    [[nodiscard]] const HandleType* address_of() const noexcept {
        return &m_handle;
    }

    [[nodiscard]] HandleType* address_of() noexcept {
        return &m_handle;
    }

    // Relinquishes ownership without destroying resource
    HandleType release() noexcept {
        HandleType temp = m_handle;
        m_handle = InvalidValue;
        return temp;
    }

    // Frees current handle and optionally takes ownership of new one
    void reset(HandleType new_handle = InvalidValue) noexcept {
        if (m_handle != InvalidValue) {
            DeleterFunc{}(m_handle);
        }
        m_handle = new_handle;
    }

    void swap(UniqueHandle& other) noexcept {
        using std::swap;
        swap(m_handle, other.m_handle);
    }

private:
    HandleType m_handle{InvalidValue};
};

template <typename H, H Inv, typename Del>
void swap(UniqueHandle<H, Inv, Del>& a, UniqueHandle<H, Inv, Del>& b) noexcept {
    a.swap(b);
}

// -------------------------------------------------------------
// Concrete Specialization 1: Standard C File Pointer (FILE*)
// -------------------------------------------------------------
struct FileCloser {
    void operator()(std::FILE* fp) const noexcept {
        if (fp != nullptr) {
            std::fclose(fp);
        }
    }
};

using UniqueFile = UniqueHandle<std::FILE*, nullptr, FileCloser>;

// Helper factory
[[nodiscard]] inline UniqueFile open_file(const char* filename, const char* mode) noexcept {
    return UniqueFile(std::fopen(filename, mode));
}

// -------------------------------------------------------------
// Concrete Specialization 2: POSIX File Descriptor (int)
// -------------------------------------------------------------
#ifndef _WIN32
#include <unistd.h>
struct PosixFdCloser {
    void operator()(int fd) const noexcept {
        if (fd >= 0) {
            ::close(fd);
        }
    }
};
using UniqueFd = UniqueHandle<int, -1, PosixFdCloser>;
#endif

} // namespace patterns::raii
