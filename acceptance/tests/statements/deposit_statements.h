#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "clients/application/application_client.h"
#include "clients/application/dto/deposit/deposit_response.h"

class DepositStatements {
public:
    explicit DepositStatements(ApplicationClient& client) : client_(client) {}

    void given_authenticated_user_has_active_session(const std::string& user, const std::string& pin) {
        const auto login = client_.login({user, pin});
        ASSERT_EQ(login.status_code, 200) << "login must succeed before the deposit is attempted";
        ASSERT_FALSE(login.token.empty()) << "authenticated user must have a session token";
        token_ = login.token;
    }

    void when_user_deposits_amount(double amount) {
        last_response_ = client_.deposit(token_, amount);
    }

    void when_unauthenticated_user_attempts_to_deposit(double amount) {
        last_response_ = client_.deposit_without_token(amount);
    }

    void then_authenticated_user_receives_updated_balance(double expected_balance) {
        EXPECT_THAT(
            last_response_,
            testing::AllOf(
                testing::Field(&dto::deposit::DepositResponse::status_code, 200),
                testing::Field(&dto::deposit::DepositResponse::error, testing::IsEmpty()),
                testing::Field(&dto::deposit::DepositResponse::balance, testing::DoubleEq(expected_balance))
            )
        );
    }

    void then_unauthenticated_request_is_rejected_with_authentication_error() {
        EXPECT_THAT(
            last_response_,
            testing::AllOf(
                testing::Field(&dto::deposit::DepositResponse::status_code, 401),
                testing::Field(&dto::deposit::DepositResponse::error, testing::Eq("Authentication error"))
            )
        );
    }

private:
    ApplicationClient& client_;
    std::string token_;
    dto::deposit::DepositResponse last_response_;
};
