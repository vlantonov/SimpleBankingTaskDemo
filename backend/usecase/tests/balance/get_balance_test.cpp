#include <gtest/gtest.h>
#include "application_test.h"
#include "statements/balance_statements.h"

class BalanceTest : public ApplicationTest {
protected:
    BalanceStatements balance_statements_;
};

TEST_F(BalanceTest, balance_request_without_a_valid_session_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: Balance request without a valid session is rejected");
    balance_statements_.assert_balance_request_without_a_valid_session_is_rejected();
}

TEST_F(BalanceTest, balance_request_with_a_fabricated_token_is_rejected) {
    SCOPED_TRACE("Scenario 1.2: Balance request with a fabricated token is rejected");
    balance_statements_.assert_balance_request_with_a_fabricated_token_is_rejected();
}

TEST_F(BalanceTest, authenticated_user_receives_the_current_balance) {
    SCOPED_TRACE("Scenario 2.1: Authenticated user receives the current balance");
    balance_statements_.assert_authenticated_user_receives_the_current_balance();
}
