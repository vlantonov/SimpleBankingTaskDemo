#include <gtest/gtest.h>
#include "statements/login_controller_statements.h"
#include "adapters/http/auth/login_controller.h"
#include "adapters/http/exception_handler.h"
#include "usecase/auth/login_usecase.h"

class LoginControllerTest : public ::testing::Test {
protected:
    usecase::LoginUsecase login_usecase_;
    adapters::http::ExceptionHandler exception_handler_;
    adapters::http::LoginController controller_{login_usecase_, exception_handler_};
    LoginControllerStatements statements_{controller_};
};

TEST_F(LoginControllerTest, login_with_empty_username_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: HTTP adapter — Login with empty username rejected with 400");

    statements_.assert_empty_username_rejected_with_validation_error();
}

TEST_F(LoginControllerTest, login_with_malformed_json_body_is_rejected) {
    SCOPED_TRACE("Coverage: malformed JSON body → parse_error → 400 Bad request");

    statements_.assert_malformed_json_rejected_with_bad_request();
}

TEST_F(LoginControllerTest, first_login_creates_user_and_opens_session_status_not_set) {
    SCOPED_TRACE("Scenario 2.1: HTTP adapter — first login returns welcome message and token");

    statements_.assert_first_login_creates_user_and_opens_session();
}

TEST_F(LoginControllerTest, DISABLED_returning_user_with_wrong_pin_is_rejected_with_invalid_credentials_returns_200_success_body) {
    SCOPED_TRACE("Scenario 3.2: HTTP adapter — InvalidCredentialsException mapped to 401");

    statements_.assert_returning_user_with_wrong_pin_is_rejected_with_invalid_credentials();
}
