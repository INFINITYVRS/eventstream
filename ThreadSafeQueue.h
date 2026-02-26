#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>

// Thread-safe bounded queue with optional push timeout.
// - `push(value, timeout)` returns true if the value was enqueued before timeout.
// - `pop()` blocks until an element is available and returns it.
// - `size()` returns current queue size.

template<typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(size_t capacity = SIZE_MAX)
        : m_capacity(capacity) {}

    // Push with optional timeout. If timeout == 0, block until space is available.
    bool push(T value, std::chrono::milliseconds timeout = std::chrono::milliseconds::zero()) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_capacity != SIZE_MAX) {
            if (timeout == std::chrono::milliseconds::zero()) {
                m_not_full.wait(lock, [this]() { return m_queue.size() < m_capacity; });
            } else {
                if (!m_not_full.wait_for(lock, timeout, [this]() { return m_queue.size() < m_capacity; })) {
                    return false; // timeout
                }
            }
        }

        m_queue.push(std::move(value));
        lock.unlock();
        m_not_empty.notify_one();
        return true;
    }

    // Blocking pop
    T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_not_empty.wait(lock, [this]() { return !m_queue.empty(); });
        T value = std::move(m_queue.front());
        m_queue.pop();
        lock.unlock();
        m_not_full.notify_one();
        return value;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_not_empty;
    std::condition_variable m_not_full;
    size_t m_capacity{SIZE_MAX};
};
