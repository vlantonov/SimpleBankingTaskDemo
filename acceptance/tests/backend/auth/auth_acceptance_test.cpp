#include <gtest/gtest.h>
#include "backend/backend_test_fixture.h"
#include "clients/application/application_client.h"
#include "statements/auth_statements.h"

class AuthTest : public BackendTestFixture {
protected:
    void SetUp() override {
        BackendTestFixture::SetUp();
        client_ = std::make_unique<ApplicationClient>(backend_host_, backend_port_);
        statements_ = std::make_unique<AuthStatements>(*client_);
    }

    std::unique_ptr<ApplicationClient> client_;
    std::unique_ptr<AuthStatements> statements_;
};

TEST_F(AuthTest, login_with_empty_username_is_rejected) {
    SCOPED_TRACE("Scenario 1.1: Login with empty username is rejected");

    statements_->when_user_logs_in("", "123");
    statements_->then_login_is_rejected_with_validation_error();
}

TEST_F(AuthTest, first_login_creates_the_user_and_opens_a_session_returns_400_bad_request) {
    SCOPED_TRACE("Scenario 2.1: First login creates the user and opens a session");

    statements_->when_user_logs_in("ola", "123");
    statements_->then_first_login_creates_user_and_opens_session("ola");
}

TEST_F(AuthTest, first_login_appends_user_created_and_login_events_to_the_log) {
    SCOPED_TRACE("Scenario 2.2: First login appends USER_CREATED and LOGIN events to the log");

    statements_->when_user_logs_in("ola", "123");
    statements_->then_first_login_appends_user_created_and_login_events("ola");
}

TEST_F(AuthTest, DISABLED_returning_user_with_correct_pin_receives_welcome_and_token) {
    SCOPED_TRACE("Scenario 3.1: Returning user with correct PIN receives welcome message and new token");

    statements_->given_user_already_exists_with_pin("ola", "123");
    statements_->when_user_logs_in("ola", "123");
    statements_->then_returning_user_receives_welcome_and_token("ola");
    statements_->then_login_event_is_recorded_for("ola");
}

TEST_F(AuthTest, returning_user_with_wrong_pin_is_rejected) {
    SCOPED_TRACE("Scenario 3.2: Returning user with wrong PIN is rejected and no session is created");

    statements_->given_user_already_exists_with_pin("ola", "123");
    statements_->when_user_logs_in("ola", "999");
    statements_->then_login_is_rejected_with_invalid_credentials_error();
}
