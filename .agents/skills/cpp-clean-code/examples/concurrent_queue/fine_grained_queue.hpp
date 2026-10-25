#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <utility>

/**
 * @file fine_grained_queue.hpp
 * @brief Fine-Grained Thread-Safe Queue with Two Mutexes (Anthony Williams - C++ Concurrency in Action, 2nd Ed).
 *
 * Architecture Highlights:
 * 1. Two-Mutex Locking: push() modifies tail under tail_mutex; pop() modifies head under head_mutex.
 *    Producers and consumers do not contend for the same lock!
 * 2. Dummy Node Technique: Ensures head and tail never reference the exact same node when data exists.
 * 3. Exception-Safe Pop: Eliminates interface race conditions (separate front() and pop() is broken in multithreading).
 *    Returns std::shared_ptr<T> directly.
 * 4. Spurious Wakeup Guard: std::condition_variable predicate loop protects against spurious signals.
 */

namespace patterns::concurrency {

template <typename T>
class FineGrainedQueue final {
private:
    struct Node {
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;
    };

    std::mutex m_head_mutex;
    std::unique_ptr<Node> m_head;

    std::mutex m_tail_mutex;
    Node* m_tail;

    std::condition_variable m_data_cond;

    [[nodiscard]] Node* get_tail() {
        std::lock_guard<std::mutex> tail_lock(m_tail_mutex);
        return m_tail;
    }

    [[nodiscard]] std::unique_ptr<Node> pop_head() {
        std::lock_guard<std::mutex> head_lock(m_head_mutex);
        if (m_head.get() == get_tail()) {
            return nullptr; // Queue is empty
        }
        std::unique_ptr<Node> old_head = std::move(m_head);
        m_head = std::move(old_head->next);
        return old_head;
    }

public:
    // Initialize with a single dummy node
    FineGrainedQueue()
        : m_head(std::make_unique<Node>())
        , m_tail(m_head.get())
    {
    }

    // Thread-safe types must disable copy and assignment
    FineGrainedQueue(const FineGrainedQueue&) = delete;
    FineGrainedQueue& operator=(const FineGrainedQueue&) = delete;

    // Enqueue an item (only locks m_tail_mutex)
    void push(T new_value) {
        auto new_data = std::make_shared<T>(std::move(new_value));
        auto p = std::make_unique<Node>();
        Node* const new_tail = p.get();

        {
            std::lock_guard<std::mutex> tail_lock(m_tail_mutex);
            m_tail->data = std::move(new_data);
            m_tail->next = std::move(p);
            m_tail = new_tail;
        }

        m_data_cond.notify_one();
    }

    // Non-blocking dequeue
    [[nodiscard]] std::shared_ptr<T> try_pop() {
        std::unique_ptr<Node> old_head = pop_head();
        return old_head ? old_head->data : nullptr;
    }

    // Blocking dequeue with condition variable
    [[nodiscard]] std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> head_lock(m_head_mutex);
        m_data_cond.wait(head_lock, [&] { return m_head.get() != get_tail(); });

        std::unique_ptr<Node> old_head = std::move(m_head);
        m_head = std::move(old_head->next);
        return old_head->data;
    }

    [[nodiscard]] bool empty() {
        std::lock_guard<std::mutex> head_lock(m_head_mutex);
        return m_head.get() == get_tail();
    }
};

} // namespace patterns::concurrency
