#include <gtest/gtest.h>
#include <memory>
#include <unordered_map>
#include "adapters/http/auth/logout_controller.h"
#include "adapters/http/exception_handler.h"
#include "statements/logout_controller_statements.h"
#include "usecase/auth/logout_usecase.h"

namespace {

class FakeLogoutSessionPort : public usecase::ILogoutSessionPort {
public:
    void given_active_session(const std::string& token, const std::string& username) {
        sessions_[token] = username;
    }

    std::optional<std::string> find_username_by_token(const std::string& token) const override {
        const auto it = sessions_.find(token);
        if (it == sessions_.end()) {
            return std::nullopt;
        }

        return it->second;
    }

    void close_session(const std::string&) override {}

private:
    std::unordered_map<std::string, std::string> sessions_;
};

class FakeAuthEventLogPort : public usecase::IAuthEventLogPort {
public:
    void append(const usecase::AuthEvent&) override {}
};

}

class LogoutControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        initialize_logout_usecase();
        controller_ = std::make_unique<adapters::http::LogoutController>(*logout_usecase_, exception_handler_);
        statements_ = std::make_unique<LogoutControllerStatements>(*controller_, active_session_initializer());
    }

    void initialize_logout_usecase() {
        auto fake_session_port = std::make_unique<FakeLogoutSessionPort>();
        fake_logout_session_port_ = fake_session_port.get();
        logout_usecase_ = std::make_unique<usecase::LogoutUsecase>(
            std::move(fake_session_port),
            std::make_unique<FakeAuthEventLogPort>());
    }

    auto active_session_initializer() {
        return [this](const std::string& token, const std::string& username) {
            fake_logout_session_port_->given_active_session(token, username);
        };
    }

    FakeLogoutSessionPort* fake_logout_session_port_ = nullptr;
    std::unique_ptr<usecase::LogoutUsecase> logout_usecase_;
    adapters::http::ExceptionHandler exception_handler_;
    std::unique_ptr<adapters::http::LogoutController> controller_;
    std::unique_ptr<LogoutControllerStatements> statements_;
};

TEST_F(LogoutControllerTest, logout_without_session_token_is_rejected_with_authentication_error) {
    SCOPED_TRACE("Scenario 5.2: HTTP adapter - logout without token returns 401 Authentication error");

    statements_->when_logout_requested_without_authorization();
    statements_->assert_authentication_error_response();
}

TEST_F(LogoutControllerTest, DISABLED_logout_with_malformed_authorization_bearer_variants_is_rejected_with_authentication_error) {
    SCOPED_TRACE("Scenario 5.2 coverage: malformed Authorization bearer variants return HTTP 401");

    statements_->given_active_session("active.header.signature", "demo-user");

    statements_->when_logout_requested_with_wrong_authorization_prefix();
    statements_->assert_authentication_error_response();

    statements_->when_logout_requested_with_empty_bearer_payload();
    statements_->assert_authentication_error_response();

    statements_->when_logout_requested_with_internal_bearer_suffix_variant();
    statements_->assert_authentication_error_response();
}
