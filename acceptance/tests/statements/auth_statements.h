#pragma once
#include <gtest/gtest.h>
#include "clients/application/application_client.h"
#include "clients/application/dto/auth/login_request.h"
#include "clients/application/dto/auth/login_response.h"

class AuthStatements {
public:
    explicit AuthStatements(ApplicationClient& client) : client_(client) {}

    void assert_login_rejected_with_validation_error(const std::string& user, const std::string& pin) {
        dto::auth::LoginRequest req{user, pin};
        auto response = client_.login(req);
        EXPECT_EQ(response.status_code, 400);
    }

private:
    ApplicationClient& client_;
};
