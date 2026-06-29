#pragma once
#include <gtest/gtest.h>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "adapters/http/auth/login_controller.h"
#include "adapters/http/exception_handler.h"

class LoginControllerStatements {
public:
    LoginControllerStatements(adapters::http::LoginController& controller,
                              const adapters::http::ExceptionHandler& handler)
        : controller_(controller)
        , handler_(handler) {}

    void assert_empty_username_rejected_with_validation_error() {
        auto res = send_empty_username_login();

        EXPECT_EQ(res.status, 400) << "status code";
        EXPECT_EQ(res.get_header_value("Content-Type"), "application/json") << "Content-Type header";
        auto body = nlohmann::json::parse(res.body);
        EXPECT_EQ(body.value("error", ""), "Validation error") << "error field";
        EXPECT_EQ(body.value("token", "PRESENT"), "") << "token must be absent";
        EXPECT_EQ(body.value("message", "PRESENT"), "") << "message must be absent";
    }

private:
    httplib::Response send_empty_username_login() {
        httplib::Request req;
        req.body = R"({"user":"","pin":"123"})";
        req.set_header("Content-Type", "application/json");
        httplib::Response res;
        controller_.handle_login(req, res);
        return res;
    }

    adapters::http::LoginController& controller_;
    const adapters::http::ExceptionHandler& handler_;
};
