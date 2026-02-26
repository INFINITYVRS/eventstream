#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <string>
#include "ThreadSafeQueue.h"
#include "WorkerPool.h"

// Compatibility shim: provides std::string_view if the system headers lack it.
#include "compat_string_view.h"

// Use a single-header HTTP library: cpp-httplib (place `httplib.h` in project root)
#include "httplib.h"

int main() {
    // Configuration
    const size_t queue_capacity = 1000;
    const size_t worker_threads = std::max<size_t>(1, std::thread::hardware_concurrency());
    const size_t http_threads = std::max<size_t>(4, worker_threads);

    ThreadSafeQueue<std::string> queue(queue_capacity);
    WorkerPool pool(worker_threads, queue);

    httplib::Server app;
    app.new_task_queue = [http_threads] {
        return new httplib::ThreadPool(http_threads);
    };

    app.Get("/health", [&](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
        res.set_content("OK", "text/plain");
    });

    app.Get("/stats", [&](const httplib::Request&, httplib::Response& res) {
        const auto total = pool.getTotalProcessed();
        const auto qsize = queue.size();
        std::string body = "{\"total_processed\":" + std::to_string(total) +
                           ",\"queue_size\":" + std::to_string(qsize) + "}";
        res.set_content(body, "application/json");
        res.status = 200;
    });

    app.Post("/ingest", [&](const httplib::Request& req, httplib::Response& res) {
        // Accept either JSON {"event":"..."} or raw body text for speed.
        std::string evt;
        const auto &body = req.body;
        const auto key = "\"event\"";
        const auto pos = body.find(key);
        if (pos != std::string::npos) {
            auto colon = body.find(':', pos + key.size());
            if (colon != std::string::npos) {
                auto start = body.find('"', colon);
                if (start != std::string::npos) {
                    auto end = body.find('"', start + 1);
                    if (end != std::string::npos) {
                        evt = body.substr(start + 1, end - start - 1);
                    }
                }
            }
        }
        if (evt.empty()) {
            evt = body;
        }
        if (evt.empty()) {
            res.status = 400;
            res.set_content("missing event\n", "text/plain");
            return;
        }

        bool ok = queue.push(std::move(evt), std::chrono::milliseconds(200));
        if (!ok) {
            res.status = 503;
            res.set_content("queue full\n", "text/plain");
            return;
        }

        res.status = 202;
        res.set_content("accepted", "text/plain");
    });

    std::cout << "Starting EventStream on port 18080 with " << worker_threads << " workers and " << http_threads << " HTTP threads\n";
    app.port(18080).multithreaded().run();
    return 0;
}