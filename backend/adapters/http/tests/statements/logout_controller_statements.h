#pragma once
#include <gtest/gtest.h>
#include <functional>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <optional>
#include "adapters/http/auth/logout_controller.h"

class LogoutControllerStatements {
public:
    explicit LogoutControllerStatements(
        adapters::http::LogoutController& controller,
        std::function<void(const std::string&, const std::string&)> given_active_session)
        : controller_(controller),
          given_active_session_(std::move(given_active_session)) {}

    void given_active_session(const std::string& token, const std::string& username) {
        given_active_session_(token, username);
    }

    void when_logout_requested_without_authorization() {
        response_ = send_logout_request_without_authorization();
    }

    void when_logout_requested_with_wrong_authorization_prefix() {
        response_ = send_logout_request_with_authorization("Token abc-session-token");
    }

    void when_logout_requested_with_empty_bearer_payload() {
        response_ = send_logout_request_with_authorization("Bearer ");
    }

    void when_logout_requested_with_internal_bearer_suffix_variant() {
        response_ = send_logout_request_with_authorization("Bearer active.header.signature");
    }

    void assert_authentication_error_response() {
        ASSERT_TRUE(response_.has_value()) << "logout response must be captured before assertions";
        const auto expected_body = nlohmann::json{{"error", "Authentication error"}, {"message", ""}, {"token", ""}};
        assert_json_response(*response_, 401, expected_body);
    }

private:
    void assert_json_response(const httplib::Response& response, int expected_status, const nlohmann::json& expected_body) {
        EXPECT_EQ(response.status, expected_status) << "status code";
        EXPECT_EQ(response.get_header_value("Content-Type"), "application/json") << "Content-Type header";
        EXPECT_EQ(nlohmann::json::parse(response.body), expected_body) << "response body";
    }

    httplib::Response send_logout_request_without_authorization() {
        return send_logout_request(std::nullopt);
    }

    httplib::Response send_logout_request_with_authorization(const std::string& authorization) {
        return send_logout_request(authorization);
    }

    httplib::Response send_logout_request(const std::optional<std::string>& authorization) {
        httplib::Request req;
        if (authorization.has_value()) {
            req.set_header("Authorization", authorization.value());
        }
        httplib::Response res;
        controller_.handle_logout(req, res);
        return res;
    }

    adapters::http::LogoutController& controller_;
    std::function<void(const std::string&, const std::string&)> given_active_session_;
    std::optional<httplib::Response> response_;
};
