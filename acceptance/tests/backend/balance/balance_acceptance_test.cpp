#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include "backend/backend_test_fixture.h"
#include "clients/application/application_client.h"
#include "statements/balance_statements.h"

class BalanceTest : public BackendTestFixture {
protected:
    void SetUp() override {
        BackendTestFixture::SetUp();
        client_ = std::make_unique<ApplicationClient>(backend_host_, backend_port_);
        statements_ = std::make_unique<BalanceStatements>(*client_);
    }

    std::unique_ptr<ApplicationClient> client_;
    std::unique_ptr<BalanceStatements> statements_;
};

TEST_F(BalanceTest, authenticated_user_receives_the_current_balance) {
    SCOPED_TRACE("Scenario 2.1: Authenticated user receives the current balance");

    const auto user = std::string{"ola_balance_"} + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());

    statements_->given_authenticated_user_has_active_session(user, "123");
    statements_->when_user_requests_balance();
    statements_->then_authenticated_user_receives_current_balance(250.00);
}

TEST_F(BalanceTest, unauthenticated_balance_request_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: Unauthenticated balance request is rejected");

    statements_->when_unauthenticated_user_requests_balance();
    statements_->then_unauthenticated_request_is_rejected_with_authentication_error();
}
