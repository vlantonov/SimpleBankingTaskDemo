#include <gtest/gtest.h>
#include "application_test.h"
#include "statements/withdraw_statements.h"

class WithdrawTest : public ApplicationTest {
protected:
    WithdrawStatements withdraw_statements_;
};

TEST_F(WithdrawTest, withdraw_request_without_a_valid_session_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: Withdraw request without a valid session is rejected");
    withdraw_statements_.assert_withdraw_request_without_a_valid_session_is_rejected();
}

TEST_F(WithdrawTest, withdraw_request_with_a_fabricated_token_is_rejected) {
    SCOPED_TRACE("Scenario 1.2: Withdraw request with a fabricated token is rejected");
    withdraw_statements_.assert_withdraw_request_with_a_fabricated_token_is_rejected();
}

TEST_F(WithdrawTest, authenticated_user_can_withdraw_a_valid_positive_amount) {
    SCOPED_TRACE("Scenario 2.1: Authenticated user can withdraw a valid positive amount");
    withdraw_statements_.assert_authenticated_user_can_withdraw_a_valid_positive_amount();
}
