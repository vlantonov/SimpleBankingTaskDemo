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
        then_login_is_rejected_with_error(400, "Validation error");
    }

    void then_login_succeeds_with_welcome_message(const std::string& user) {
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

    void then_first_login_creates_user_and_opens_session(const std::string& user) {
        then_login_succeeds_with_welcome_message(user);
    }

    void then_first_login_appends_user_created_and_login_events(const std::string& user) {
        const auto log_contents = read_log_contents();
        assert_event_in_log(log_contents, "USER_CREATED", "USER_CREATED event must be recorded");
        assert_event_in_log(log_contents, user, "user name must be recorded in the log");
        assert_event_in_log(log_contents, "LOGIN", "LOGIN event must be recorded");

        const auto user_created_position = log_contents.find("USER_CREATED");
        const auto login_position = log_contents.find("LOGIN");
        EXPECT_LT(user_created_position, login_position) << "USER_CREATED must be logged before LOGIN";
    }

    void given_user_already_exists_with_pin(const std::string& user, const std::string& pin) {
        when_user_logs_in(user, pin);
    }

    void then_returning_user_receives_welcome_and_token(const std::string& user) {
        then_login_succeeds_with_welcome_message(user);
    }

    void then_login_event_is_recorded_for(const std::string& user) {
        const auto log_contents = read_log_contents();
        assert_event_in_log(log_contents, "LOGIN", "LOGIN event must be recorded");
        assert_event_in_log(log_contents, user, "user name must be recorded in the log");
    }

    void then_login_is_rejected_with_invalid_credentials_error() {
        then_login_is_rejected_with_error(401, "Invalid credentials");
    }

    static std::string read_log_contents() {
        const auto log_path = std::filesystem::path{"auth_events.log"};
        std::ifstream log_file(log_path);
        if (!log_file.is_open()) {
            return {};
        }

        return std::string{std::istreambuf_iterator<char>(log_file), std::istreambuf_iterator<char>()};
    }

private:
    void then_login_is_rejected_with_error(int status_code, const std::string& error) {
        EXPECT_THAT(
            last_response_,
            testing::AllOf(
                testing::Field(&dto::auth::LoginResponse::status_code, status_code),
                testing::Field(&dto::auth::LoginResponse::error, testing::Eq(error)),
                testing::Field(&dto::auth::LoginResponse::token, testing::IsEmpty()),
                testing::Field(&dto::auth::LoginResponse::message, testing::IsEmpty())
            )
        );
    }

    void assert_event_in_log(const std::string& log_contents, const std::string& event,
                             const std::string& error_message) {
        const auto position = log_contents.find(event);
        ASSERT_NE(position, std::string::npos) << error_message;
    }

    ApplicationClient& client_;
    dto::auth::LoginResponse last_response_;
};
