#include "autotimer.hpp"

#include <chrono>
#include <iostream>
#include <string>

namespace patterns::pimpl {

// Full definition of Impl is hidden completely inside this translation unit (.cpp).
// Any platform-specific or heavy dependencies belong here, shielded from consumers.
class AutoTimer::Impl final {
public:
    using Clock = std::chrono::steady_clock;

    explicit Impl(std::string_view name)
        : m_name(name)
        , m_start_time(Clock::now())
    {
    }

    ~Impl() {
        // Automatically prints elapsed duration on destruction
        std::cout << "[Timer '" << m_name << "'] Elapsed: " 
                  << elapsed_seconds() << " seconds\n";
    }

    [[nodiscard]] double elapsed_seconds() const {
        const auto now = Clock::now();
        const std::chrono::duration<double> diff = now - m_start_time;
        return diff.count();
    }

    [[nodiscard]] std::string_view name() const noexcept {
        return m_name;
    }

    void reset() {
        m_start_time = Clock::now();
    }

private:
    std::string m_name;
    Clock::time_point m_start_time;
};

// Constructor: Uses std::make_unique to instantiate Impl
AutoTimer::AutoTimer(std::string_view timer_name)
    : m_impl(std::make_unique<Impl>(timer_name))
{
}

// Destructor defined HERE: At this point, Impl is a complete type,
// allowing std::default_delete<Impl> to compile safely!
AutoTimer::~AutoTimer() = default;

// Move operations defined HERE: Impl is complete.
AutoTimer::AutoTimer(AutoTimer&&) noexcept = default;
AutoTimer& AutoTimer::operator=(AutoTimer&&) noexcept = default;

// Interface forwarding to Impl
double AutoTimer::elapsed_seconds() const {
    return m_impl->elapsed_seconds();
}

std::string_view AutoTimer::name() const noexcept {
    return m_impl->name();
}

void AutoTimer::reset() {
    m_impl->reset();
}

} // namespace patterns::pimpl
