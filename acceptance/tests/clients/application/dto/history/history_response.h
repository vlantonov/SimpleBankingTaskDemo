#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace dto::history {

struct HistoryEntry {
    std::string type;
    double amount{};
    std::string timestamp;
};

struct HistoryResponse {
    int status_code{};
    std::vector<HistoryEntry> transactions;
    std::string error;

    static HistoryResponse parse(int status_code, const std::string& body) {
        HistoryResponse response;
        response.status_code = status_code;
        try {
            const auto json = nlohmann::json::parse(body);
            response.error = json.value("error", std::string{});
            for (const auto& item : json.value("transactions", nlohmann::json::array())) {
                HistoryEntry entry;
                entry.type = item.value("type", std::string{});
                entry.amount = item.value("amount", 0.0);
                entry.timestamp = item.value("timestamp", std::string{});
                response.transactions.push_back(entry);
            }
        } catch (...) {
        }
        return response;
    }

    static HistoryResponse connection_failed() {
        return HistoryResponse{0, {}, "Connection failed"};
    }
};

}
