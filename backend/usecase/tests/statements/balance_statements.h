#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>
#include "domain/balance/balance_request.h"
#include "domain/domain_exception.h"
#include "usecase/balance/get_balance_usecase.h"

namespace balance_statements_detail {

constexpr auto kValidBalanceToken = "ola.header.signature";
constexpr auto kBalanceUsername = "ola";
constexpr auto kBalanceAmount = 250.0;

class ActiveSessionPortForBalanceStatements final : public usecase::IGetBalanceSessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string& token) const override {
        if (token == kValidBalanceToken) {
            return kBalanceUsername;
        }
        return std::nullopt;
    }
};

class RejectingSessionPortForBalanceStatements final : public usecase::IGetBalanceSessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string&) const override {
        return std::nullopt;
    }
};

class BalanceAccountPortForBalanceStatements final : public usecase::IAccountPort {
public:
    std::optional<double> find_balance_for_username(const std::string& username) const override {
        if (username == kBalanceUsername) {
            return kBalanceAmount;
        }
        return std::nullopt;
    }
};

}  // namespace balance_statements_detail

class BalanceStatements {
public:
    void assert_balance_request_without_a_valid_session_is_rejected() {
        auto session_port = std::make_unique<balance_statements_detail::RejectingSessionPortForBalanceStatements>();
        auto account_port = std::make_unique<balance_statements_detail::BalanceAccountPortForBalanceStatements>();
        usecase::GetBalanceUsecase usecase(std::move(session_port), std::move(account_port));

        EXPECT_THAT([&]() { usecase.execute(domain::BalanceRequest{"missing-token"}); },
                    testing::ThrowsMessage<domain::InvalidCredentialsException>(testing::Eq("Authentication error")));
    }

    void assert_balance_request_with_a_fabricated_token_is_rejected() {
        auto session_port = std::make_unique<balance_statements_detail::RejectingSessionPortForBalanceStatements>();
        auto account_port = std::make_unique<balance_statements_detail::BalanceAccountPortForBalanceStatements>();
        usecase::GetBalanceUsecase usecase(std::move(session_port), std::move(account_port));

        EXPECT_THAT([&]() { usecase.execute(domain::BalanceRequest{"fabricated.token"}); },
                    testing::ThrowsMessage<domain::InvalidCredentialsException>(testing::Eq("Authentication error")));
    }

    void assert_authenticated_user_receives_the_current_balance() {
        auto session_port = std::make_unique<balance_statements_detail::ActiveSessionPortForBalanceStatements>();
        auto account_port = std::make_unique<balance_statements_detail::BalanceAccountPortForBalanceStatements>();
        usecase::GetBalanceUsecase usecase(std::move(session_port), std::move(account_port));

        const auto response = usecase.execute(domain::BalanceRequest{balance_statements_detail::kValidBalanceToken});
        EXPECT_THAT(response.balance, testing::DoubleEq(balance_statements_detail::kBalanceAmount));
    }
};
