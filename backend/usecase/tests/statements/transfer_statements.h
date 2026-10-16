#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>
#include "domain/domain_exception.h"
#include "domain/transfer/transfer_request.h"
#include "usecase/transfer/transfer_usecase.h"

namespace transfer_statements_detail {

constexpr auto kValidTransferToken = "ola.header.signature";
constexpr auto kSourceUsername = "ola";
constexpr auto kDestinationUsername = "marta";
constexpr auto kSourceStartingBalance = 200.0;
constexpr auto kDestinationStartingBalance = 50.0;
constexpr auto kTransferAmount = 40.0;
constexpr auto kExpectedSourceBalance = 160.0;
constexpr auto kExpectedDestinationBalance = 90.0;

class ActiveSessionPortForTransferStatements final : public usecase::ITransferSessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string& token) const override {
        if (token == kValidTransferToken) {
            return kSourceUsername;
        }
        return std::nullopt;
    }
};

class RejectingSessionPortForTransferStatements final : public usecase::ITransferSessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string&) const override {
        return std::nullopt;
    }
};

class TransferAccountPortForTransferStatements final : public usecase::ITransferAccountPort {
public:
    std::optional<double> find_balance_for_username(const std::string& username) const override {
        if (username == kSourceUsername) {
            return source_balance_;
        }
        if (username == kDestinationUsername) {
            return destination_balance_;
        }
        return std::nullopt;
    }

    bool destination_exists(const std::string& username) const override {
        return username == kDestinationUsername;
    }

    void append_transfer_between(const std::string& source_username,
                                 const std::string& destination_username,
                                 double amount) override {
        if (source_username == kSourceUsername && destination_username == kDestinationUsername) {
            source_balance_ -= amount;
            destination_balance_ += amount;
        }
    }

private:
    double source_balance_{kSourceStartingBalance};
    double destination_balance_{kDestinationStartingBalance};
};

}  // namespace transfer_statements_detail

class TransferStatements {
public:
    void assert_transfer_request_without_an_active_session_is_rejected() {
        usecase::TransferUsecase usecase(
            std::make_unique<transfer_statements_detail::RejectingSessionPortForTransferStatements>(),
            build_account_port());

        EXPECT_THAT([&]() {
            usecase.execute(domain::TransferRequest{"missing-token", transfer_statements_detail::kDestinationUsername, 25.0});
        }, testing::ThrowsMessage<domain::InvalidCredentialsException>(testing::Eq("Authentication error")));
    }

    void when_authenticated_user_transfers_funds_to_another_existing_user() {
        actual_response_ = build_usecase().execute(build_transfer_request());
    }

    void assert_transfer_updates_both_account_balances() const {
        ASSERT_EQ(actual_response_, std::optional<usecase::TransferResponse>{expected_transfer_response()});
    }

private:
    static std::unique_ptr<usecase::ITransferAccountPort> build_account_port() {
        return std::make_unique<transfer_statements_detail::TransferAccountPortForTransferStatements>();
    }

    static domain::TransferRequest build_transfer_request() {
        return domain::TransferRequest{
            transfer_statements_detail::kValidTransferToken,
            transfer_statements_detail::kDestinationUsername,
            transfer_statements_detail::kTransferAmount
        };
    }

    static usecase::TransferUsecase build_usecase() {
        return usecase::TransferUsecase(
            std::make_unique<transfer_statements_detail::ActiveSessionPortForTransferStatements>(),
            build_account_port());
    }

    static usecase::TransferResponse expected_transfer_response() {
        return usecase::TransferResponse{
            transfer_statements_detail::kExpectedSourceBalance,
            transfer_statements_detail::kExpectedDestinationBalance
        };
    }

    std::optional<usecase::TransferResponse> actual_response_;
};
