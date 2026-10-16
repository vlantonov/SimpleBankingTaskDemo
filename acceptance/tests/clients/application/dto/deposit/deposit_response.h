#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace dto::deposit {

struct DepositResponse {
    int status_code{};
    double balance{};
    std::string error;
    std::string message;

    static DepositResponse parse(int status_code, const std::string& body) {
        DepositResponse response;
        response.status_code = status_code;
        try {
            const auto json = nlohmann::json::parse(body);
            response.balance = json.value("balance", 0.0);
            response.error = json.value("error", std::string{});
            response.message = json.value("message", std::string{});
        } catch (...) {
        }
        return response;
    }

    static DepositResponse connection_failed() {
        return DepositResponse{0, 0.0, "Connection failed", ""};
    }
};

}
