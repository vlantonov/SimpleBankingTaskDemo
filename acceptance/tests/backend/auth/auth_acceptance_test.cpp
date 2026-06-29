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

    statements_->assert_login_rejected_with_validation_error("", "123");
}
