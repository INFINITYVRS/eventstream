#pragma once

#include <vector>
#include <thread>
#include <string>
#include <atomic>
#include "ThreadSafeQueue.h"

// WorkerPool consumes string events from a ThreadSafeQueue and processes them.
class WorkerPool {
public:
    WorkerPool(size_t numThreads, ThreadSafeQueue<std::string> &queue)
        : m_queue(queue), m_done(false), m_totalProcessed(0) {
        start(numThreads);
    }

    ~WorkerPool() {
        stop();
    }

    size_t getTotalProcessed() const { return m_totalProcessed.load(std::memory_order_relaxed); }
    size_t getQueueSize() const { return m_queue.size(); }

private:
    void start(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            m_threads.emplace_back([this]() {
                while (!m_done) {
                    std::string evt = m_queue.pop();
                    if (m_done) break;
                    process(evt);
                }
            });
        }
    }

    void process(const std::string &evt) {
        // Lightweight processing: update count only.
        (void)evt;
        m_totalProcessed.fetch_add(1, std::memory_order_relaxed);
    }

    void stop() {
        m_done = true;
        // Push sentinel empty events to wake up workers.
        for (size_t i = 0; i < m_threads.size(); ++i) {
            // keep trying until we can push a sentinel to wake the thread
            while (!m_queue.push(std::string(), std::chrono::milliseconds(100))) {}
        }

        for (auto &t : m_threads) {
            if (t.joinable()) t.join();
        }
    }

    std::vector<std::thread> m_threads;
    ThreadSafeQueue<std::string> &m_queue;
    std::atomic<bool> m_done;
    std::atomic<size_t> m_totalProcessed;
};
