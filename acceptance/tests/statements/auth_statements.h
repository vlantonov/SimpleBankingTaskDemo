#pragma once
#include <gtest/gtest.h>
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
        EXPECT_EQ(last_response_.status_code, 400) << "status code";
        EXPECT_EQ(last_response_.error, "Validation error") << "error message";
        EXPECT_EQ(last_response_.token, "") << "token must be absent on rejection";
        EXPECT_EQ(last_response_.message, "") << "message must be absent on rejection";
    }

    void then_first_login_creates_user_and_opens_session(const std::string& user) {
        EXPECT_EQ(last_response_.status_code, 200) << "status code";
        EXPECT_EQ(last_response_.error, "") << "error must be absent on success";
        EXPECT_EQ(last_response_.message, "Welcome, " + user) << "welcome message";
        assert_jwt_like_token(last_response_.token);
    }

private:
    static void assert_jwt_like_token(const std::string& token) {
        EXPECT_FALSE(token.empty()) << "token must be present";
        assert_jwt_separator_count(token);
        assert_jwt_segment_content(token);
    }

    static void assert_jwt_separator_count(const std::string& token) {
        EXPECT_NE(first_dot_index(token), std::string::npos)
            << "token must contain first JWT separator";
        EXPECT_NE(second_dot_index(token), std::string::npos)
            << "token must contain second JWT separator";
        EXPECT_EQ(third_dot_index(token), std::string::npos)
            << "token must contain exactly two JWT separators";
    }

    static void assert_jwt_segment_content(const std::string& token) {
        EXPECT_GT(first_dot_index(token), 0U)
            << "token header segment must be non-empty";
        EXPECT_GT(second_dot_index(token), first_dot_index(token) + 1)
            << "token payload segment must be non-empty";
        EXPECT_LT(second_dot_index(token) + 1, token.size())
            << "token signature segment must be non-empty";
    }

    static std::size_t first_dot_index(const std::string& token) {
        return token.find('.');
    }

    static std::size_t second_dot_index(const std::string& token) {
        return token.find('.', first_dot_index(token) == std::string::npos ? 0 : first_dot_index(token) + 1);
    }

    static std::size_t third_dot_index(const std::string& token) {
        return token.find('.', second_dot_index(token) == std::string::npos ? 0 : second_dot_index(token) + 1);
    }

    ApplicationClient& client_;
    dto::auth::LoginResponse last_response_;
};
