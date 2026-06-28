#pragma once
#include <httplib.h>
#include <string>
#include "dto/auth/login_request.h"
#include "dto/auth/login_response.h"

class ApplicationClient {
public:
    explicit ApplicationClient(const std::string& host, int port)
        : client_(host, port) {}

    dto::auth::LoginResponse login(const dto::auth::LoginRequest& req) {
        auto res = client_.Post("/api/v1/login", req.to_json_string(), "application/json");
        if (!res) return dto::auth::LoginResponse::connection_failed();
        return dto::auth::LoginResponse::parse(res->status, res->body);
    }

private:
    httplib::Client client_;
};
