#include <chrono>
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

TEST_F(AuthTest, login_with_excessively_long_username_is_rejected) {
    SCOPED_TRACE("Scenario S3.1: Excessively long username is rejected");

    auto long_username = std::string{"long_user_"} + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    long_username.resize(257, 'a');
    statements_->when_user_logs_in(long_username, "123");
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

TEST_F(AuthTest, second_login_while_a_session_is_already_active_is_rejected) {
    SCOPED_TRACE("Scenario 4.1: Second login while a session is already active is rejected");

    statements_->given_user_already_exists_with_pin("ola", "123");
    statements_->when_user_logs_in("ola", "123");
    statements_->when_user_logs_in("ola", "123");
    statements_->then_login_is_rejected_with_session_conflict_error();
}

TEST_F(AuthTest, logout_with_a_valid_session_token_destroys_the_session) {
    SCOPED_TRACE("Scenario 5.1: Logout with a valid session token destroys the session");

    statements_->given_user_already_exists_with_pin("ola", "123");
    statements_->when_user_logs_in("ola", "123");
    statements_->when_user_logs_out_using_the_session_token();
    statements_->then_logout_succeeds();
    statements_->then_session_token_is_no_longer_valid();
    statements_->then_logout_event_is_recorded_for("ola");
}

TEST_F(AuthTest, logout_without_a_session_token_is_rejected) {
    SCOPED_TRACE("Scenario 5.2: Logout without a session token is rejected");

    statements_->when_unauthenticated_caller_attempts_to_log_out();
    statements_->then_logout_is_rejected_with_authentication_error();
}

TEST_F(AuthTest, session_tokens_for_independent_logins_are_unique_and_not_derived_from_username) {
    SCOPED_TRACE("Scenario S2.2: Session token is sufficiently random");

    const auto suffix = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    const auto first_user = std::string{"random_token_user_"} + suffix;
    const auto second_user = std::string{"random_token_user_"} + suffix + "_2";

    const auto first_login = client_->login(dto::auth::LoginRequest{first_user, "123"});
    const auto second_login = client_->login(dto::auth::LoginRequest{second_user, "123"});

    ASSERT_EQ(first_login.status_code, 200) << "first independent login should succeed";
    ASSERT_EQ(second_login.status_code, 200) << "second independent login should succeed";
    EXPECT_NE(first_login.token, second_login.token) << "distinct logins should return distinct tokens";
    EXPECT_THAT(first_login.token, testing::Not(testing::HasSubstr(first_user)))
        << "token must not be derived from the username";
    EXPECT_THAT(second_login.token, testing::Not(testing::HasSubstr(second_user)))
        << "token must not be derived from the username";
}

TEST_F(AuthTest, repeated_failed_login_attempts_are_rate_limited) {
    SCOPED_TRACE("Scenario S1.1: Repeated failed login attempts are rate-limited");

    const auto user = "rate_limited_user_" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());

    statements_->given_user_already_exists_with_pin(user, "123");
    statements_->when_user_submits_multiple_failed_login_attempts_for(user, 6);
    statements_->then_failed_login_attempts_for_user_are_throttled();
}
