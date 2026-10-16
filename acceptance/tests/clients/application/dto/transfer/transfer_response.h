#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace dto::transfer {

struct TransferResponse {
    int status_code{};
    double source_balance{};
    double destination_balance{};
    std::string error;
    std::string message;

    bool operator==(const TransferResponse& other) const {
        return status_code == other.status_code
            && source_balance == other.source_balance
            && destination_balance == other.destination_balance
            && error == other.error
            && message == other.message;
    }

    static TransferResponse parse(int status_code, const std::string& body) {
        TransferResponse response;
        response.status_code = status_code;
        try {
            const auto json = nlohmann::json::parse(body);
            response.source_balance = json.value("sourceBalance", 0.0);
            response.destination_balance = json.value("destinationBalance", 0.0);
            response.error = json.value("error", std::string{});
            response.message = json.value("message", std::string{});
        } catch (...) {
        }
        return response;
    }

    static TransferResponse connection_failed() {
        return TransferResponse{0, 0.0, 0.0, "Connection failed", ""};
    }
};

}
