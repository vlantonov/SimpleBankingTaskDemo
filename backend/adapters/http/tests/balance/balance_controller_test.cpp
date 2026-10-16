#include <gtest/gtest.h>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include "adapters/http/balance/balance_controller.h"
#include "adapters/http/exception_handler.h"
#include "statements/balance_controller_statements.h"
#include "usecase/balance/get_balance_usecase.h"

namespace {

class FakeBalanceSessionPort : public usecase::IGetBalanceSessionPort {
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

private:
    std::unordered_map<std::string, std::string> sessions_;
};

class FakeBalanceAccountPort : public usecase::IAccountPort {
public:
    std::optional<double> find_balance_for_username(const std::string& username) const override {
        if (username == "ola") {
            return 250.0;
        }
        return std::nullopt;
    }
};

}

class BalanceControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        initialize_balance_usecase();
        controller_ = std::make_unique<adapters::http::BalanceController>(*usecase_, exception_handler_);
        statements_ = std::make_unique<BalanceControllerStatements>(*controller_, active_session_initializer());
    }

    void initialize_balance_usecase() {
        auto session_port = std::make_unique<FakeBalanceSessionPort>();
        fake_balance_session_port_ = session_port.get();

        usecase_ = std::make_unique<usecase::GetBalanceUsecase>(
            std::move(session_port),
            std::make_unique<FakeBalanceAccountPort>());
    }

    std::function<void(const std::string&, const std::string&)> active_session_initializer() {
        return [this](const std::string& token, const std::string& username) {
            fake_balance_session_port_->given_active_session(token, username);
        };
    }

    FakeBalanceSessionPort* fake_balance_session_port_ = nullptr;
    std::unique_ptr<usecase::GetBalanceUsecase> usecase_;
    adapters::http::ExceptionHandler exception_handler_;
    std::unique_ptr<adapters::http::BalanceController> controller_;
    std::unique_ptr<BalanceControllerStatements> statements_;
};

TEST_F(BalanceControllerTest, balance_without_session_token_is_rejected_with_authentication_error) {
    SCOPED_TRACE("Scenario 1.1: HTTP adapter — balance without token returns 401 Authentication error");

    statements_->when_balance_requested_without_authorization();
    statements_->assert_authentication_error_response();
}

TEST_F(BalanceControllerTest, authenticated_user_receives_the_current_balance) {
    SCOPED_TRACE("Scenario 2.1: HTTP adapter — valid balance request returns the current balance");

    statements_->given_active_session("ola.header.signature", "ola");
    statements_->when_balance_requested_with_authorization("Bearer ola.header.signature");
    statements_->assert_balance_response(250.0);
}
