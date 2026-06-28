#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace dto::auth {

struct LoginResponse {
    std::string message;
    std::string token;
    std::string error;
    int status_code{};

    static LoginResponse connection_failed() {
        LoginResponse r;
        r.status_code = 0;
        r.error = "Connection failed";
        return r;
    }

    static LoginResponse parse(int status, const std::string& body) {
        LoginResponse r;
        r.status_code = status;
        try_parse_fields(r, body);
        return r;
    }

private:
    static void try_parse_fields(LoginResponse& r, const std::string& body) {
        try {
            apply_json_fields(r, nlohmann::json::parse(body));
        } catch (...) {}
    }

    static void apply_json_fields(LoginResponse& r, const nlohmann::json& json) {
        r.message = json.value("message", std::string{});
        r.token = json.value("token", std::string{});
        r.error = json.value("error", std::string{});
    }
};

}
