#pragma once
#include <gtest/gtest.h>
#include <functional>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>
#include "adapters/http/history/history_controller.h"

class HistoryControllerStatements {
public:
    explicit HistoryControllerStatements(
        adapters::http::HistoryController& controller,
        std::function<void(const std::string&, const std::string&)> given_active_session)
        : controller_(controller),
          given_active_session_(std::move(given_active_session)) {}

    void given_active_session(const std::string& token, const std::string& username) {
        given_active_session_(token, username);
    }

    void when_history_requested_without_authorization() {
        response_ = send_history_request(std::nullopt);
    }

    void when_history_requested_with_authorization(const std::string& authorization) {
        response_ = send_history_request(authorization);
    }

    void assert_authentication_error_response() {
        ASSERT_TRUE(response_.has_value()) << "history response must be captured before assertions";
        const auto expected_body = nlohmann::json{{"error", "Authentication error"}, {"message", ""}, {"token", ""}};
        assert_json_response(*response_, 401, expected_body);
    }

    void assert_history_response(const std::vector<std::tuple<std::string, double, std::string>>& expected_entries) {
        ASSERT_TRUE(response_.has_value()) << "history response must be captured before assertions";

        nlohmann::json expected_body = nlohmann::json::object();
        expected_body["transactions"] = nlohmann::json::array();
        for (const auto& [type, amount, timestamp] : expected_entries) {
            expected_body["transactions"].push_back({
                {"type", type},
                {"amount", amount},
                {"timestamp", timestamp}
            });
        }

        assert_json_response(*response_, 200, expected_body);
    }

private:
    void assert_json_response(const httplib::Response& response, int expected_status, const nlohmann::json& expected_body) {
        EXPECT_EQ(response.status, expected_status) << "status code";
        EXPECT_EQ(response.get_header_value("Content-Type"), "application/json") << "Content-Type header";
        EXPECT_EQ(nlohmann::json::parse(response.body), expected_body) << "response body";
    }

    httplib::Response send_history_request(const std::optional<std::string>& authorization) {
        httplib::Request req;
        if (authorization.has_value()) {
            req.set_header("Authorization", authorization.value());
        }
        httplib::Response res;
        controller_.handle_history(req, res);
        return res;
    }

    adapters::http::HistoryController& controller_;
    std::function<void(const std::string&, const std::string&)> given_active_session_;
    std::optional<httplib::Response> response_;
};
