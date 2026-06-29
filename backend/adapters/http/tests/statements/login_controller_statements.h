#pragma once
#include <gtest/gtest.h>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "adapters/http/auth/login_controller.h"

class LoginControllerStatements {
public:
    LoginControllerStatements(adapters::http::LoginController& controller)
        : controller_(controller) {}

    void assert_empty_username_rejected_with_validation_error() {
        auto res = send_login_request(R"({"user":"","pin":"123"})");

        EXPECT_EQ(res.status, 400) << "status code";
        EXPECT_EQ(res.get_header_value("Content-Type"), "application/json") << "Content-Type header";
        auto body = nlohmann::json::parse(res.body);
        EXPECT_EQ(body.value("error", ""), "Validation error") << "error field";
        EXPECT_EQ(body.value("token", "PRESENT"), "") << "token must be absent";
        EXPECT_EQ(body.value("message", "PRESENT"), "") << "message must be absent";
    }

    void assert_malformed_json_rejected_with_bad_request() {
        auto res = send_login_request("not valid json {{{");

        EXPECT_EQ(res.status, 400) << "status code";
        EXPECT_EQ(res.get_header_value("Content-Type"), "application/json") << "Content-Type header";
        auto body = nlohmann::json::parse(res.body);
        EXPECT_EQ(body.value("error", ""), "Bad request") << "error field";
        EXPECT_EQ(body.value("token", "PRESENT"), "") << "token must be absent";
        EXPECT_EQ(body.value("message", "PRESENT"), "") << "message must be absent";
    }

private:
    httplib::Response send_login_request(const std::string& body) {
        httplib::Request req;
        req.body = body;
        req.set_header("Content-Type", "application/json");
        httplib::Response res;
        controller_.handle_login(req, res);
        return res;
    }

    adapters::http::LoginController& controller_;
};
