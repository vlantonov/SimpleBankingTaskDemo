#include <gtest/gtest.h>
#include "application_test.h"
#include "statements/history_statements.h"

class HistoryTest : public ApplicationTest {
protected:
    HistoryStatements history_statements_;
};

TEST_F(HistoryTest, history_request_without_a_valid_session_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: History request without a valid session is rejected");
    history_statements_.assert_history_request_without_a_valid_session_is_rejected();
}

TEST_F(HistoryTest, history_request_with_a_fabricated_token_is_rejected) {
    SCOPED_TRACE("Scenario 1.2: History request with a fabricated token is rejected");
    history_statements_.assert_history_request_with_a_fabricated_token_is_rejected();
}

TEST_F(HistoryTest, authenticated_user_receives_their_transaction_history) {
    SCOPED_TRACE("Scenario 2.1: Authenticated user receives their transaction history");
    history_statements_.assert_authenticated_user_receives_their_transaction_history();
}
