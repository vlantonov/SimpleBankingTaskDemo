#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace dto::withdraw {

struct WithdrawResponse {
    int status_code{};
    double balance{};
    std::string error;
    std::string message;

    static WithdrawResponse parse(int status_code, const std::string& body) {
        WithdrawResponse response;
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

    static WithdrawResponse connection_failed() {
        return WithdrawResponse{0, 0.0, "Connection failed", ""};
    }
};

}
