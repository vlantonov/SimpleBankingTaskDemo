#pragma once
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <string>
#include "dto/auth/login_request.h"
#include "dto/auth/login_response.h"

class ApplicationClient {
public:
    explicit ApplicationClient(const std::string& host, int port)
        : client_(host, port) {}

    dto::auth::LoginResponse login(const dto::auth::LoginRequest& req) {
        nlohmann::json body;
        body["user"] = req.user;
        body["pin"] = req.pin;

        auto res = client_.Post("/api/v1/login", body.dump(), "application/json");

        dto::auth::LoginResponse response;
        if (!res) {
            response.status_code = 0;
            response.error = "Connection failed";
            return response;
        }

        response.status_code = res->status;
        if (!res->body.empty()) {
            try {
                auto json = nlohmann::json::parse(res->body);
                if (json.contains("message")) response.message = json["message"];
                if (json.contains("token")) response.token = json["token"];
                if (json.contains("error")) response.error = json["error"];
            } catch (...) {}
        }
        return response;
    }

private:
    httplib::Client client_;
};
