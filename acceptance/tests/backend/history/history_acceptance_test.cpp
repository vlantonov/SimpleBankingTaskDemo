#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include "backend/backend_test_fixture.h"
#include "clients/application/application_client.h"
#include "statements/history_statements.h"

class HistoryTest : public BackendTestFixture {
protected:
    void SetUp() override {
        BackendTestFixture::SetUp();
        client_ = std::make_unique<ApplicationClient>(backend_host_, backend_port_);
        statements_ = std::make_unique<HistoryStatements>(*client_);
    }

    std::unique_ptr<ApplicationClient> client_;
    std::unique_ptr<HistoryStatements> statements_;
};

TEST_F(HistoryTest, unauthenticated_history_request_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: Unauthenticated history request is rejected");

    statements_->when_unauthenticated_user_requests_history();
    statements_->then_unauthenticated_request_is_rejected_with_authentication_error();
}

TEST_F(HistoryTest, authenticated_user_can_retrieve_their_transaction_history) {
    SCOPED_TRACE("Scenario 3.1: Authenticated user can retrieve their transaction history");

    const auto user = std::string{"ola_history_"} + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());

    statements_->given_authenticated_user_has_active_session(user, "123");
    statements_->when_user_requests_history();
    statements_->then_authenticated_user_receives_transaction_history();
}
