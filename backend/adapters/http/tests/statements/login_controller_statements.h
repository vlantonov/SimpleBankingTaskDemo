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
        assert_error_response(
            send_login_request(R"({"user":"","pin":"123"})"),
            "Validation error"
        );
    }

    void assert_malformed_json_rejected_with_bad_request() {
        assert_error_response(send_login_request("not valid json {{{"), "Bad request");
    }

    void assert_first_login_creates_user_and_opens_session() {
        auto res = send_login_request(R"({"user":"ola","pin":"123"})");

        ASSERT_EQ(res.status, 200) << "status code";
        ASSERT_EQ(res.get_header_value("Content-Type"), "application/json") << "Content-Type header";
        const auto body = nlohmann::json::parse(res.body);
        const auto expected_body = nlohmann::json{{"message", "Welcome, ola"}, {"token", "ola-session-token"}, {"error", ""}};
        EXPECT_EQ(body, expected_body) << "response body";
    }

private:
    void assert_error_response(const httplib::Response& response, const std::string& error) {
        EXPECT_EQ(response.status, 400) << "status code";
        EXPECT_EQ(response.get_header_value("Content-Type"), "application/json") << "Content-Type header";
        const auto body = nlohmann::json::parse(response.body);
        const auto expected_body = nlohmann::json{{"error", error}, {"token", ""}, {"message", ""}};
        EXPECT_EQ(body, expected_body) << "response body";
    }

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
