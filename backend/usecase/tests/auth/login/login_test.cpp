#include <gtest/gtest.h>
#include "application_test.h"

class LoginTest : public ApplicationTest {};

TEST_F(LoginTest, DISABLED_login_with_empty_username_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: Login with empty username is rejected");
    auth_statements_.assert_login_rejected_with_validation_error();
}
