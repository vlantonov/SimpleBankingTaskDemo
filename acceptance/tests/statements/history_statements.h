#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "clients/application/application_client.h"
#include "clients/application/dto/history/history_response.h"

class HistoryStatements {
public:
    explicit HistoryStatements(ApplicationClient& client) : client_(client) {}

    void given_authenticated_user_has_active_session(const std::string& user, const std::string& pin) {
        const auto login = client_.login({user, pin});
        ASSERT_EQ(login.status_code, 200) << "login must succeed before history is requested";
        ASSERT_FALSE(login.token.empty()) << "authenticated user must have a session token";
        token_ = login.token;
    }

    void when_user_requests_history() {
        last_response_ = client_.history(token_);
    }

    void when_unauthenticated_user_requests_history() {
        last_response_ = client_.history_without_token();
    }

    void then_authenticated_user_receives_transaction_history() {
        EXPECT_THAT(
            last_response_,
            testing::AllOf(
                testing::Field(&dto::history::HistoryResponse::status_code, 200),
                testing::Field(&dto::history::HistoryResponse::error, testing::IsEmpty()),
                testing::Field(&dto::history::HistoryResponse::transactions, testing::SizeIs(2))
            )
        );
    }

    void then_unauthenticated_request_is_rejected_with_authentication_error() {
        EXPECT_THAT(
            last_response_,
            testing::AllOf(
                testing::Field(&dto::history::HistoryResponse::status_code, 401),
                testing::Field(&dto::history::HistoryResponse::error, testing::Eq("Authentication error"))
            )
        );
    }

private:
    ApplicationClient& client_;
    std::string token_;
    dto::history::HistoryResponse last_response_;
};
