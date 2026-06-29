#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "clients/application/application_client.h"
#include "clients/application/dto/auth/login_request.h"
#include "clients/application/dto/auth/login_response.h"

class AuthStatements {
public:
    explicit AuthStatements(ApplicationClient& client) : client_(client) {}

    void when_user_logs_in(const std::string& user, const std::string& pin) {
        last_response_ = client_.login(dto::auth::LoginRequest{user, pin});
    }

    void then_login_is_rejected_with_validation_error() {
        EXPECT_THAT(
            last_response_,
            testing::AllOf(
                testing::Field(&dto::auth::LoginResponse::status_code, 400),
                testing::Field(&dto::auth::LoginResponse::error, testing::Eq("Validation error")),
                testing::Field(&dto::auth::LoginResponse::token, testing::IsEmpty()),
                testing::Field(&dto::auth::LoginResponse::message, testing::IsEmpty())
            )
        );
    }

    void then_first_login_creates_user_and_opens_session(const std::string& user) {
        EXPECT_THAT(
            last_response_,
            testing::AllOf(
                testing::Field(&dto::auth::LoginResponse::status_code, 200),
                testing::Field(&dto::auth::LoginResponse::error, testing::IsEmpty()),
                testing::Field(&dto::auth::LoginResponse::message, testing::Eq("Welcome, " + user)),
                testing::Field(&dto::auth::LoginResponse::token, testing::Not(testing::IsEmpty()))
            )
        );
    }

    void then_first_login_appends_user_created_and_login_events(const std::string& user) {
        const auto log_contents = read_log_contents();
        const auto user_created_position = log_contents.find("USER_CREATED");
        const auto user_position = log_contents.find(user);
        const auto login_position = log_contents.find("LOGIN");

        ASSERT_NE(user_created_position, std::string::npos) << "USER_CREATED event must be recorded";
        ASSERT_NE(user_position, std::string::npos) << "user name must be recorded in the log";
        ASSERT_NE(login_position, std::string::npos) << "LOGIN event must be recorded";
        EXPECT_LT(user_created_position, login_position) << "USER_CREATED must be logged before LOGIN";
    }

    static std::string read_log_contents() {
        const auto log_path = std::filesystem::path{"auth_events.log"};
        std::ifstream log_file(log_path);
        if (!log_file.is_open()) {
            return {};
        }

        return std::string{std::istreambuf_iterator<char>(log_file), std::istreambuf_iterator<char>()};
    }

    ApplicationClient& client_;
    dto::auth::LoginResponse last_response_;
};
