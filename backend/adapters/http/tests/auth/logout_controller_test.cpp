#include <gtest/gtest.h>
#include <memory>
#include "adapters/http/auth/logout_controller.h"
#include "adapters/http/exception_handler.h"
#include "statements/logout_controller_statements.h"
#include "usecase/auth/logout_usecase.h"

namespace {

class FakeLogoutSessionPort : public usecase::ILogoutSessionPort {
public:
    std::optional<std::string> find_username_by_token(const std::string&) const override {
        return std::nullopt;
    }

    void close_session(const std::string&) override {}
};

class FakeAuthEventLogPort : public usecase::IAuthEventLogPort {
public:
    void append(const usecase::AuthEvent&) override {}
};

}

class LogoutControllerTest : public ::testing::Test {
protected:
    usecase::LogoutUsecase logout_usecase_{
        std::make_unique<FakeLogoutSessionPort>(),
        std::make_unique<FakeAuthEventLogPort>()};
    adapters::http::ExceptionHandler exception_handler_;
    adapters::http::LogoutController controller_{logout_usecase_, exception_handler_};
    LogoutControllerStatements statements_{controller_};
};

TEST_F(LogoutControllerTest, DISABLED_logout_without_session_token_is_rejected_with_authentication_error) {
    SCOPED_TRACE("Scenario 5.2: HTTP adapter - logout without token returns 401 Authentication error");

    statements_.when_logout_requested_without_authorization();
    statements_.assert_authentication_error_response();
}
