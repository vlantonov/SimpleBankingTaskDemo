#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include "backend/backend_test_fixture.h"
#include "clients/application/application_client.h"
#include "statements/deposit_statements.h"

class DepositTest : public BackendTestFixture {
protected:
    void SetUp() override {
        BackendTestFixture::SetUp();
        client_ = std::make_unique<ApplicationClient>(backend_host_, backend_port_);
        statements_ = std::make_unique<DepositStatements>(*client_);
    }

    std::unique_ptr<ApplicationClient> client_;
    std::unique_ptr<DepositStatements> statements_;
};

TEST_F(DepositTest, unauthenticated_deposit_request_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: Unauthenticated deposit request is rejected");

    statements_->when_unauthenticated_user_attempts_to_deposit(75.50);
    statements_->then_unauthenticated_request_is_rejected_with_authentication_error();
}

TEST_F(DepositTest, authenticated_user_can_deposit_a_valid_positive_amount) {
    SCOPED_TRACE("Scenario 2.1: Authenticated user can deposit a valid positive amount");

    const auto user = std::string{"ola_deposit_"} + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());

    statements_->given_authenticated_user_has_active_session(user, "123");
    statements_->when_user_deposits_amount(75.50);
    statements_->then_authenticated_user_receives_updated_balance(325.50);
}
