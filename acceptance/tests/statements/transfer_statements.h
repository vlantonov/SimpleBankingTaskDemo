#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "clients/application/application_client.h"
#include "clients/application/dto/transfer/transfer_response.h"
#include "clients/application/dto/withdraw/withdraw_response.h"

class TransferStatements {
public:
    explicit TransferStatements(ApplicationClient& client) : client_(client) {}

    void given_authenticated_source_user_and_existing_destination_with_balances(
        const std::string& source_user,
        const std::string& source_pin,
        const std::string& destination_user,
        const std::string& destination_pin
    ) {
        destination_user_name_ = destination_user;

        login_with_token(source_user, source_pin, "source", source_token_);

        std::string destination_token;
        login_with_token(destination_user, destination_pin, "destination", destination_token);

        assert_withdraw_success(source_token_, 50.0, "source balance setup must succeed");
        assert_withdraw_success(destination_token, 200.0, "destination balance setup must succeed");
    }

    void when_source_user_transfers_amount_to_destination(double amount) {
        last_transfer_response_ = client_.transfer(source_token_, destination_user_name_, amount);
    }

    void then_transfer_is_accepted_and_balances_are_updated(double expected_source_balance,
                                                             double expected_destination_balance) {
        const dto::transfer::TransferResponse expected_response{
            201,
            expected_source_balance,
            expected_destination_balance,
            "",
            "Transfer successful"
        };
        EXPECT_EQ(last_transfer_response_, expected_response) << "transfer response payload";
    }

private:
    void login_with_token(const std::string& user,
                          const std::string& pin,
                          const std::string& user_label,
                          std::string& token_out) {
        const auto login = client_.login({user, pin});
        ASSERT_EQ(login.status_code, 200) << user_label << " login must succeed before transfer";
        ASSERT_FALSE(login.token.empty()) << user_label << " user must have a session token";
        token_out = login.token;
    }

    void assert_withdraw_success(const std::string& token,
                                 double amount,
                                 const std::string& message) {
        const auto withdraw = client_.withdraw(token, amount);
        ASSERT_EQ(withdraw.status_code, 200) << message;
    }

    ApplicationClient& client_;
    std::string source_token_;
    std::string destination_user_name_;
    dto::transfer::TransferResponse last_transfer_response_;
};
