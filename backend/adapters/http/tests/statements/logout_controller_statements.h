#pragma once
#include <gtest/gtest.h>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <optional>
#include "adapters/http/auth/logout_controller.h"

class LogoutControllerStatements {
public:
    explicit LogoutControllerStatements(adapters::http::LogoutController& controller)
        : controller_(controller) {}

    void when_logout_requested_without_authorization() {
        response_ = send_logout_request_without_authorization();
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
        httplib::Request req;
        httplib::Response res;
        controller_.handle_logout(req, res);
        return res;
    }

    adapters::http::LogoutController& controller_;
    std::optional<httplib::Response> response_;
};
