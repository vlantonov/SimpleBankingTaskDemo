#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "clients/application/application_client.h"
#include "clients/application/dto/balance/balance_response.h"

class BalanceStatements {
public:
    explicit BalanceStatements(ApplicationClient& client) : client_(client) {}

    void given_authenticated_user_has_active_session(const std::string& user, const std::string& pin) {
        const auto login = client_.login({user, pin});
        ASSERT_EQ(login.status_code, 200) << "login must succeed before balance is checked";
        ASSERT_FALSE(login.token.empty()) << "authenticated user must have a session token";
        token_ = login.token;
    }

    void when_user_requests_balance() {
        last_balance_response_ = client_.balance(token_);
    }

    void when_unauthenticated_user_requests_balance() {
        last_balance_response_ = client_.balance_without_token();
    }

    void then_authenticated_user_receives_current_balance(double expected_balance) {
        EXPECT_THAT(
            last_balance_response_,
            testing::AllOf(
                testing::Field(&dto::balance::BalanceResponse::status_code, 200),
                testing::Field(&dto::balance::BalanceResponse::error, testing::IsEmpty()),
                testing::Field(&dto::balance::BalanceResponse::balance, testing::DoubleEq(expected_balance))
            )
        );
    }

    void then_unauthenticated_request_is_rejected_with_authentication_error() {
        EXPECT_THAT(
            last_balance_response_,
            testing::AllOf(
                testing::Field(&dto::balance::BalanceResponse::status_code, 401),
                testing::Field(&dto::balance::BalanceResponse::error, testing::Eq("Authentication error"))
            )
        );
    }

private:
    ApplicationClient& client_;
    std::string token_;
    dto::balance::BalanceResponse last_balance_response_;
};
