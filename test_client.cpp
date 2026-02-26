#include <iostream>
#include <string>
#include "httplib.h"

int main() {
    httplib::Client cli("localhost", 18080);

    // Health
    if (auto res = cli.Get("/health")) {
        std::cout << "GET /health: " << res->status << " " << res->body << "\n";
    } else {
        std::cout << "GET /health failed\n";
    }

    // Ingest
    std::string payload = "{\"event\":\"hello\"}";
    if (auto res = cli.Post("/ingest", payload, "application/json")) {
        std::cout << "POST /ingest: " << res->status << " " << res->body << "\n";
    } else {
        std::cout << "POST /ingest failed\n";
    }

    // Stats
    if (auto res = cli.Get("/stats")) {
        std::cout << "GET /stats: " << res->status << " " << res->body << "\n";
    } else {
        std::cout << "GET /stats failed\n";
    }

    return 0;
}
