#include <gtest/gtest.h>
#include "application_test.h"

class LoginTest : public ApplicationTest {};

TEST_F(LoginTest, login_with_empty_username_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: Login with empty username is rejected");
    auth_statements_.assert_login_rejected_with_validation_error();
}

TEST_F(LoginTest, overly_long_username_is_rejected) {
    SCOPED_TRACE("Scenario S3.1: Overly long username is rejected");
    auth_statements_.assert_login_with_overly_long_username_is_rejected();
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

TEST_F(LoginTest, returning_user_with_wrong_pin_is_rejected_and_no_session_is_created) {
    SCOPED_TRACE("Scenario 3.2: Returning user with wrong PIN is rejected and no session is created");
    auth_statements_.assert_returning_user_with_wrong_pin_is_rejected_without_side_effects();
}

TEST_F(LoginTest, repeated_failed_login_attempts_are_rate_limited) {
    SCOPED_TRACE("Scenario S1.1: Repeated failed login attempts are rate-limited");
    auth_statements_.assert_repeated_failed_login_attempts_are_rate_limited();
}

// TDD Red Phase - does not throw SessionConflictException and still creates side effects.
TEST_F(LoginTest, second_login_while_a_session_is_already_active_is_rejected) {
    SCOPED_TRACE("Scenario 4.1: Second login while a session is already active is rejected");
    auth_statements_.assert_second_login_is_rejected_when_session_is_already_active();
}
