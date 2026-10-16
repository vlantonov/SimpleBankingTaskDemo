#pragma once
#include <gtest/gtest.h>
#include <functional>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include "adapters/http/balance/balance_controller.h"

class BalanceControllerStatements {
public:
    explicit BalanceControllerStatements(
        adapters::http::BalanceController& controller,
        std::function<void(const std::string&, const std::string&)> given_active_session)
        : controller_(controller),
          given_active_session_(std::move(given_active_session)) {}

    void given_active_session(const std::string& token, const std::string& username) {
        given_active_session_(token, username);
    }

    void when_balance_requested_without_authorization() {
        response_ = send_balance_request(std::nullopt);
    }

    void when_balance_requested_with_authorization(const std::string& authorization) {
        response_ = send_balance_request(authorization);
    }

    void assert_authentication_error_response() {
        ASSERT_TRUE(response_.has_value()) << "balance response must be captured before assertions";
        const auto expected_body = nlohmann::json{{"error", "Authentication error"}, {"message", ""}, {"token", ""}};
        assert_json_response(*response_, 401, expected_body);
    }

    void assert_balance_response(double expected_balance) {
        ASSERT_TRUE(response_.has_value()) << "balance response must be captured before assertions";
        const auto expected_body = nlohmann::json{{"balance", expected_balance}};
        assert_json_response(*response_, 200, expected_body);
    }

private:
    void assert_json_response(const httplib::Response& response, int expected_status, const nlohmann::json& expected_body) {
        EXPECT_EQ(response.status, expected_status) << "status code";
        EXPECT_EQ(response.get_header_value("Content-Type"), "application/json") << "Content-Type header";
        EXPECT_EQ(nlohmann::json::parse(response.body), expected_body) << "response body";
    }

    httplib::Response send_balance_request(const std::optional<std::string>& authorization) {
        httplib::Request req;
        if (authorization.has_value()) {
            req.set_header("Authorization", authorization.value());
        }
        httplib::Response res;
        controller_.handle_balance(req, res);
        return res;
    }

    adapters::http::BalanceController& controller_;
    std::function<void(const std::string&, const std::string&)> given_active_session_;
    std::optional<httplib::Response> response_;
};
