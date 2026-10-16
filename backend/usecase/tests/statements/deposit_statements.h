#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>
#include "domain/deposit/deposit_request.h"
#include "domain/domain_exception.h"
#include "usecase/deposit/deposit_usecase.h"

namespace deposit_statements_detail {

constexpr auto kValidDepositToken = "ola.header.signature";
constexpr auto kDepositUsername = "ola";
constexpr auto kStartingBalance = 250.0;
constexpr auto kDepositAmount = 75.5;
constexpr auto kExpectedBalance = 325.5;

class ActiveSessionPortForDepositStatements final : public usecase::IDepositSessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string& token) const override {
        if (token == kValidDepositToken) {
            return kDepositUsername;
        }
        return std::nullopt;
    }
};

class RejectingSessionPortForDepositStatements final : public usecase::IDepositSessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string&) const override {
        return std::nullopt;
    }
};

class DepositAccountPortForDepositStatements final : public usecase::IAccountingPort {
public:
    std::optional<double> find_balance_for_username(const std::string& username) const override {
        if (username == kDepositUsername) {
            return kStartingBalance;
        }
        return std::nullopt;
    }

    void append_deposit_for_username(const std::string& username, double amount) override {
        if (username == kDepositUsername) {
            current_balance_ = kStartingBalance + amount;
        }
    }

    double current_balance_{kStartingBalance};
};

}  // namespace deposit_statements_detail

class DepositStatements {
public:
    void assert_deposit_request_without_a_valid_session_is_rejected() {
        auto session_port = std::make_unique<deposit_statements_detail::RejectingSessionPortForDepositStatements>();
        auto account_port = std::make_unique<deposit_statements_detail::DepositAccountPortForDepositStatements>();
        usecase::DepositUsecase usecase(std::move(session_port), std::move(account_port));

        EXPECT_THAT([&]() { usecase.execute(domain::DepositRequest{"missing-token", 75.5}); },
                    testing::ThrowsMessage<domain::InvalidCredentialsException>(testing::Eq("Authentication error")));
    }

    void assert_deposit_request_with_a_fabricated_token_is_rejected() {
        auto session_port = std::make_unique<deposit_statements_detail::RejectingSessionPortForDepositStatements>();
        auto account_port = std::make_unique<deposit_statements_detail::DepositAccountPortForDepositStatements>();
        usecase::DepositUsecase usecase(std::move(session_port), std::move(account_port));

        EXPECT_THAT([&]() { usecase.execute(domain::DepositRequest{"fabricated.token", 75.5}); },
                    testing::ThrowsMessage<domain::InvalidCredentialsException>(testing::Eq("Authentication error")));
    }

    void assert_authenticated_user_can_deposit_a_valid_positive_amount() {
        auto session_port = std::make_unique<deposit_statements_detail::ActiveSessionPortForDepositStatements>();
        auto account_port = std::make_unique<deposit_statements_detail::DepositAccountPortForDepositStatements>();
        usecase::DepositUsecase usecase(std::move(session_port), std::move(account_port));

        const auto response = usecase.execute(domain::DepositRequest{deposit_statements_detail::kValidDepositToken, deposit_statements_detail::kDepositAmount});
        EXPECT_THAT(response.balance, testing::DoubleEq(deposit_statements_detail::kExpectedBalance));
    }
};
