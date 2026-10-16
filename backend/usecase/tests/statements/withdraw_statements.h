#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>
#include "domain/withdraw/withdraw_request.h"
#include "domain/domain_exception.h"
#include "usecase/withdraw/withdraw_usecase.h"

namespace withdraw_statements_detail {

constexpr auto kValidWithdrawToken = "ola.header.signature";
constexpr auto kWithdrawUsername = "ola";
constexpr auto kStartingBalance = 250.0;
constexpr auto kWithdrawAmount = 75.5;
constexpr auto kExpectedBalance = 174.5;

class ActiveSessionPortForWithdrawStatements final : public usecase::IWithdrawSessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string& token) const override {
        if (token == kValidWithdrawToken) {
            return kWithdrawUsername;
        }
        return std::nullopt;
    }
};

class RejectingSessionPortForWithdrawStatements final : public usecase::IWithdrawSessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string&) const override {
        return std::nullopt;
    }
};

class WithdrawAccountPortForWithdrawStatements final : public usecase::IAccountingPort {
public:
    std::optional<double> find_balance_for_username(const std::string& username) const override {
        if (username == kWithdrawUsername) {
            return kStartingBalance;
        }
        return std::nullopt;
    }

    void append_withdraw_for_username(const std::string& username, double amount) override {
        if (username == kWithdrawUsername) {
            current_balance_ = kStartingBalance - amount;
        }
    }

    double current_balance_{kStartingBalance};
};

}  // namespace withdraw_statements_detail

class WithdrawStatements {
public:
    void assert_withdraw_request_without_a_valid_session_is_rejected() {
        auto session_port = std::make_unique<withdraw_statements_detail::RejectingSessionPortForWithdrawStatements>();
        auto account_port = std::make_unique<withdraw_statements_detail::WithdrawAccountPortForWithdrawStatements>();
        usecase::WithdrawUsecase usecase(std::move(session_port), std::move(account_port));

        EXPECT_THAT([&]() { usecase.execute(domain::WithdrawRequest{"missing-token", 75.5}); },
                    testing::ThrowsMessage<domain::InvalidCredentialsException>(testing::Eq("Authentication error")));
    }

    void assert_withdraw_request_with_a_fabricated_token_is_rejected() {
        auto session_port = std::make_unique<withdraw_statements_detail::RejectingSessionPortForWithdrawStatements>();
        auto account_port = std::make_unique<withdraw_statements_detail::WithdrawAccountPortForWithdrawStatements>();
        usecase::WithdrawUsecase usecase(std::move(session_port), std::move(account_port));

        EXPECT_THAT([&]() { usecase.execute(domain::WithdrawRequest{"fabricated.token", 75.5}); },
                    testing::ThrowsMessage<domain::InvalidCredentialsException>(testing::Eq("Authentication error")));
    }

    void assert_authenticated_user_can_withdraw_a_valid_positive_amount() {
        auto session_port = std::make_unique<withdraw_statements_detail::ActiveSessionPortForWithdrawStatements>();
        auto account_port = std::make_unique<withdraw_statements_detail::WithdrawAccountPortForWithdrawStatements>();
        usecase::WithdrawUsecase usecase(std::move(session_port), std::move(account_port));

        const auto response = usecase.execute(domain::WithdrawRequest{withdraw_statements_detail::kValidWithdrawToken, withdraw_statements_detail::kWithdrawAmount});
        EXPECT_THAT(response.balance, testing::DoubleEq(withdraw_statements_detail::kExpectedBalance));
    }
};
