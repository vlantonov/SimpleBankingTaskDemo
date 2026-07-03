#include <gtest/gtest.h>
#include "application_test.h"
#include "statements/logout_statements.h"

class LogoutTest : public ApplicationTest {
protected:
    LogoutStatements logout_statements_;
};

TEST_F(LogoutTest, logout_with_a_valid_session_token_destroys_the_session) {
    SCOPED_TRACE("Scenario 5.1: Logout with a valid session token destroys the session");
    logout_statements_.assert_logout_with_valid_session_token_destroys_the_session();
}