#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace dto::balance {

struct BalanceResponse {
    int status_code{};
    double balance{};
    std::string error;
    std::string message;

    static BalanceResponse parse(int status_code, const std::string& body) {
        BalanceResponse response;
        response.status_code = status_code;
        try {
            auto json = nlohmann::json::parse(body);
            response.balance = json.value("balance", 0.0);
            response.error = json.value("error", std::string{});
            response.message = json.value("message", std::string{});
        } catch (...) {
        }
        return response;
    }

    static BalanceResponse connection_failed() {
        return BalanceResponse{0, 0.0, "Connection failed", ""};
    }
};

}
