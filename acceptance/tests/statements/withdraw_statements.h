#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "clients/application/application_client.h"
#include "clients/application/dto/withdraw/withdraw_response.h"

class WithdrawStatements {
public:
    explicit WithdrawStatements(ApplicationClient& client) : client_(client) {}

    void given_authenticated_user_has_active_session(const std::string& user, const std::string& pin) {
        const auto login = client_.login({user, pin});
        ASSERT_EQ(login.status_code, 200) << "login must succeed before the withdrawal is attempted";
        ASSERT_FALSE(login.token.empty()) << "authenticated user must have a session token";
        token_ = login.token;
    }

    void when_user_withdraws_amount(double amount) {
        last_response_ = client_.withdraw(token_, amount);
    }

    void when_unauthenticated_user_attempts_to_withdraw(double amount) {
        last_response_ = client_.withdraw_without_token(amount);
    }

    void then_authenticated_user_receives_updated_balance(double expected_balance) {
        EXPECT_THAT(
            last_response_,
            testing::AllOf(
                testing::Field(&dto::withdraw::WithdrawResponse::status_code, 200),
                testing::Field(&dto::withdraw::WithdrawResponse::error, testing::IsEmpty()),
                testing::Field(&dto::withdraw::WithdrawResponse::balance, testing::DoubleEq(expected_balance))
            )
        );
    }

    void then_unauthenticated_request_is_rejected_with_authentication_error() {
        EXPECT_THAT(
            last_response_,
            testing::AllOf(
                testing::Field(&dto::withdraw::WithdrawResponse::status_code, 401),
                testing::Field(&dto::withdraw::WithdrawResponse::error, testing::Eq("Authentication error"))
            )
        );
    }

private:
    ApplicationClient& client_;
    std::string token_;
    dto::withdraw::WithdrawResponse last_response_;
};
