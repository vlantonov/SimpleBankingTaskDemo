#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace dto::auth {

struct LogoutResponse {
    int status_code;
    std::string error;
    std::string message;

    static LogoutResponse parse(int status_code, const std::string& body) {
        try {
            auto json = nlohmann::json::parse(body);
            return LogoutResponse{
                status_code,
                json.value("error", ""),
                json.value("message", "")
            };
        } catch (...) {
            return LogoutResponse{status_code, "", ""};
        }
    }

    static LogoutResponse connection_failed() {
        return LogoutResponse{0, "connection_failed", ""};
    }
};

}
