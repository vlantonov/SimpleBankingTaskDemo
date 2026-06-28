#pragma once
#include <gtest/gtest.h>
#include "clients/application/application_client.h"
#include "clients/application/dto/auth/login_request.h"
#include "clients/application/dto/auth/login_response.h"

class AuthStatements {
public:
    explicit AuthStatements(ApplicationClient& client) : client_(client) {}

    void assert_login_rejected_with_validation_error(const std::string& user, const std::string& pin) {
        auto response = client_.login(dto::auth::LoginRequest{user, pin});
        EXPECT_EQ(response.status_code, 400) << "status code";
        EXPECT_EQ(response.error, "Validation error") << "error message";
        EXPECT_EQ(response.token, "") << "token must be absent on rejection";
        EXPECT_EQ(response.message, "") << "message must be absent on rejection";
    }

private:
    ApplicationClient& client_;
};
