#include <gtest/gtest.h>
#include "application_test.h"
#include "statements/transfer_statements.h"

class TransferTest : public ApplicationTest {
protected:
    TransferStatements transfer_statements_;
};

TEST_F(TransferTest, DISABLED_transfer_request_without_an_active_session_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: Transfer request without an active session is rejected");
    transfer_statements_.assert_transfer_request_without_an_active_session_is_rejected();
}

TEST_F(TransferTest, authenticated_user_can_transfer_funds_to_another_existing_user_not_implemented) {
    SCOPED_TRACE("Scenario 3.1: Authenticated user can transfer funds to another existing user");
    transfer_statements_.when_authenticated_user_transfers_funds_to_another_existing_user();
    transfer_statements_.assert_transfer_updates_both_account_balances();
}
