#pragma once

#include <thread>
#include <utility>
#include <stdexcept>

/**
 * @file scoped_thread.hpp
 * @brief RAII Wrapper for std::thread (Scott Meyers - Effective Modern C++, Item 37).
 *
 * Core Problem:
 * If std::thread destructor runs while the thread is still joinable(), the C++ runtime
 * immediately calls std::terminate(), killing the application without clean unwind.
 *
 * Solution:
 * ScopedThread guarantees that join() is executed on all execution paths (normal return or exception).
 */

namespace patterns::concurrency {

enum class ThreadDtorAction {
    Join,
    Detach
};

class ScopedThread final {
public:
    explicit ScopedThread(std::thread t, ThreadDtorAction action = ThreadDtorAction::Join)
        : m_thread(std::move(t))
        , m_action(action)
    {
        if (!m_thread.joinable()) {
            throw std::invalid_argument("ScopedThread: thread must be joinable!");
        }
    }

    ~ScopedThread() noexcept {
        if (m_thread.joinable()) {
            if (m_action == ThreadDtorAction::Join) {
                m_thread.join();
            } else {
                m_thread.detach();
            }
        }
    }

    // Move-only semantics
    ScopedThread(const ScopedThread&) = delete;
    ScopedThread& operator=(const ScopedThread&) = delete;

    ScopedThread(ScopedThread&& other) noexcept
        : m_thread(std::move(other.m_thread))
        , m_action(other.m_action)
    {
    }

    ScopedThread& operator=(ScopedThread&& other) noexcept {
        if (this != &other) {
            // Join current thread if joinable before replacing
            if (m_thread.joinable()) {
                if (m_action == ThreadDtorAction::Join) {
                    m_thread.join();
                } else {
                    m_thread.detach();
                }
            }
            m_thread = std::move(other.m_thread);
            m_action = other.m_action;
        }
        return *this;
    }

    [[nodiscard]] std::thread& get() noexcept { return m_thread; }
    [[nodiscard]] const std::thread& get() const noexcept { return m_thread; }
    [[nodiscard]] bool joinable() const noexcept { return m_thread.joinable(); }

private:
    std::thread m_thread;
    ThreadDtorAction m_action;
};

} // namespace patterns::concurrency
