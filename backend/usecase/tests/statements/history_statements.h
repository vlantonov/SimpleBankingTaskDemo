#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "domain/domain_exception.h"
#include "usecase/history/get_history_usecase.h"

namespace history_statements_detail {

constexpr auto kValidHistoryToken = "ola.header.signature";
constexpr auto kHistoryUsername = "ola";

class ActiveSessionPortForHistoryStatements final : public usecase::IGetHistorySessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string& token) const override {
        if (token == kValidHistoryToken) {
            return kHistoryUsername;
        }
        return std::nullopt;
    }
};

class RejectingSessionPortForHistoryStatements final : public usecase::IGetHistorySessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string&) const override {
        return std::nullopt;
    }
};

class HistoryAccountPortForHistoryStatements final : public usecase::IHistoryAccountPort {
public:
    std::vector<usecase::HistoryEntry> find_history_for_username(const std::string& username) const override {
        if (username == kHistoryUsername) {
            return {
                usecase::HistoryEntry{"withdraw", 25.0, "2024-01-01T00:00:10Z"},
                usecase::HistoryEntry{"deposit", 100.0, "2024-01-01T00:00:00Z"}
            };
        }
        return {};
    }
};

}  // namespace history_statements_detail

class HistoryStatements {
public:
    void assert_history_request_without_a_valid_session_is_rejected() {
        auto session_port = std::make_unique<history_statements_detail::RejectingSessionPortForHistoryStatements>();
        auto account_port = std::make_unique<history_statements_detail::HistoryAccountPortForHistoryStatements>();
        usecase::GetHistoryUsecase usecase(std::move(session_port), std::move(account_port));

        EXPECT_THAT([&]() { usecase.execute(domain::HistoryRequest{"missing-token"}); },
                    testing::ThrowsMessage<domain::InvalidCredentialsException>(testing::Eq("Authentication error")));
    }

    void assert_history_request_with_a_fabricated_token_is_rejected() {
        auto session_port = std::make_unique<history_statements_detail::RejectingSessionPortForHistoryStatements>();
        auto account_port = std::make_unique<history_statements_detail::HistoryAccountPortForHistoryStatements>();
        usecase::GetHistoryUsecase usecase(std::move(session_port), std::move(account_port));

        EXPECT_THAT([&]() { usecase.execute(domain::HistoryRequest{"fabricated.token"}); },
                    testing::ThrowsMessage<domain::InvalidCredentialsException>(testing::Eq("Authentication error")));
    }

    void assert_authenticated_user_receives_their_transaction_history() {
        auto session_port = std::make_unique<history_statements_detail::ActiveSessionPortForHistoryStatements>();
        auto account_port = std::make_unique<history_statements_detail::HistoryAccountPortForHistoryStatements>();
        usecase::GetHistoryUsecase usecase(std::move(session_port), std::move(account_port));

        const auto response = usecase.execute(domain::HistoryRequest{history_statements_detail::kValidHistoryToken});
        ASSERT_EQ(response.entries.size(), 2);
        EXPECT_EQ(response.entries[0].type, "withdraw");
        EXPECT_DOUBLE_EQ(response.entries[0].amount, 25.0);
        EXPECT_EQ(response.entries[1].type, "deposit");
        EXPECT_DOUBLE_EQ(response.entries[1].amount, 100.0);
    }
};
