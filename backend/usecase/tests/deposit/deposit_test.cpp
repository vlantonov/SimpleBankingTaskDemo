#include <gtest/gtest.h>
#include "application_test.h"
#include "statements/deposit_statements.h"

class DepositTest : public ApplicationTest {
protected:
    DepositStatements deposit_statements_;
};

TEST_F(DepositTest, deposit_request_without_a_valid_session_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: Deposit request without a valid session is rejected");
    deposit_statements_.assert_deposit_request_without_a_valid_session_is_rejected();
}

TEST_F(DepositTest, deposit_request_with_a_fabricated_token_is_rejected) {
    SCOPED_TRACE("Scenario 1.2: Deposit request with a fabricated token is rejected");
    deposit_statements_.assert_deposit_request_with_a_fabricated_token_is_rejected();
}

TEST_F(DepositTest, authenticated_user_can_deposit_a_valid_positive_amount) {
    SCOPED_TRACE("Scenario 2.1: Authenticated user can deposit a valid positive amount");
    deposit_statements_.assert_authenticated_user_can_deposit_a_valid_positive_amount();
}
