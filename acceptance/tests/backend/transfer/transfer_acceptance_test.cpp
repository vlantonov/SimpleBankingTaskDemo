#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include "backend/backend_test_fixture.h"
#include "clients/application/application_client.h"
#include "statements/transfer_statements.h"

class TransferTest : public BackendTestFixture {
protected:
    struct TransferUsers {
        std::string source;
        std::string destination;
    };

    void SetUp() override {
        BackendTestFixture::SetUp();
        client_ = std::make_unique<ApplicationClient>(backend_host_, backend_port_);
        statements_ = std::make_unique<TransferStatements>(*client_);
    }

    TransferUsers unique_transfer_users() const {
        const auto suffix = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        return {std::string{"ola_transfer_"} + suffix, std::string{"marta_transfer_"} + suffix};
    }

    std::unique_ptr<ApplicationClient> client_;
    std::unique_ptr<TransferStatements> statements_;
};

TEST_F(TransferTest, DISABLED_authenticated_user_can_transfer_funds_to_another_existing_user) {
    SCOPED_TRACE("Scenario 3.1: Authenticated user can transfer funds to another existing user");

    const auto users = unique_transfer_users();

    statements_->given_authenticated_source_user_and_existing_destination_with_balances(
        users.source,
        "123",
        users.destination,
        "123"
    );
    statements_->when_source_user_transfers_amount_to_destination(40.0);
    statements_->then_transfer_is_accepted_and_balances_are_updated(160.0, 90.0);
}
