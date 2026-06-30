#include <gtest/gtest.h>
#include "application_test.h"

class LoginTest : public ApplicationTest {};

TEST_F(LoginTest, login_with_empty_username_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: Login with empty username is rejected");
    auth_statements_.assert_login_rejected_with_validation_error();
}

// TDD Red Phase - C++ exception with description "Not implemented" thrown in the test body.
TEST_F(LoginTest, first_login_creates_the_user_and_opens_a_session) {
    SCOPED_TRACE("Scenario 2.1: First login creates the user and opens a session");
    auth_statements_.assert_first_login_creates_user_and_opens_session();
}

TEST_F(LoginTest, first_login_appends_user_created_and_login_events_to_the_log_captured_events_size_is_0_instead_of_2) {
    SCOPED_TRACE("Scenario 2.2: First login appends USER_CREATED and LOGIN events to the log");
    auth_statements_.assert_first_login_appends_user_created_and_login_events_to_log();
}
