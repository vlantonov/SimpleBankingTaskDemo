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

        const auto expected_body = nlohmann::json{{"message", "Welcome, ola"}, {"token", "ola-session-token"}, {"error", ""}};
        assert_json_response(res, 200, expected_body);
    }

    void assert_returning_user_with_wrong_pin_is_rejected_with_invalid_credentials() {
        send_login_request(R"({"user":"ola","pin":"123"})");
        assert_error_response(send_login_request(R"({"user":"ola","pin":"999"})"), "Invalid credentials", 401);
    }

private:
    void assert_json_response(const httplib::Response& response, int expected_status, const nlohmann::json& expected_body) {
        EXPECT_EQ(response.status, expected_status) << "status code";
        EXPECT_EQ(response.get_header_value("Content-Type"), "application/json") << "Content-Type header";
        EXPECT_EQ(nlohmann::json::parse(response.body), expected_body) << "response body";
    }

    void assert_error_response(const httplib::Response& response, const std::string& error, int expected_status = 400) {
        const auto expected_body = nlohmann::json{{"error", error}, {"token", ""}, {"message", ""}};
        assert_json_response(response, expected_status, expected_body);
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
