#pragma once

#include <memory>
#include <string_view>

/**
 * @file autotimer.hpp
 * @brief Modern C++ Pimpl (Pointer to Implementation) Idiom Example.
 *
 * Key Principles Demonstrated:
 * 1. Physical Decoupling: Zero platform-specific headers (<windows.h>, <sys/time.h>) in public API.
 * 2. Stable ABI: Size of AutoTimer remains constant (size of one std::unique_ptr).
 * 3. Incomplete Type Handling (Scott Meyers Item 22):
 *    - Destructor, move constructor, and move assignment MUST be declared here,
 *      but defined in .cpp where sizeof(Impl) is known to default_delete.
 * 4. Const-Correctness: Accessors must preserve logical constness even though m_impl is a pointer.
 */

namespace patterns::pimpl {

class AutoTimer final {
public:
    // Explicit constructor prevents accidental implicit conversions
    explicit AutoTimer(std::string_view timer_name);

    // DESTRUCTOR: Must be defined in .cpp!
    // std::unique_ptr requires complete type for destruction.
    ~AutoTimer();

    // RULE OF FIVE:
    // Disable copying because this timer owns unique timing resources.
    AutoTimer(const AutoTimer&) = delete;
    AutoTimer& operator=(const AutoTimer&) = delete;

    // Move operations: Must be declared here, defined in .cpp.
    AutoTimer(AutoTimer&&) noexcept;
    AutoTimer& operator=(AutoTimer&&) noexcept;

    // Const-correct public interface
    [[nodiscard]] double elapsed_seconds() const;
    [[nodiscard]] std::string_view name() const noexcept;
    void reset();

private:
    class Impl;                        // Forward declaration of incomplete implementation type
    std::unique_ptr<Impl> m_impl;       // Pimpl pointer managing lifetime via RAII
};

} // namespace patterns::pimpl
