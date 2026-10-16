#include <gtest/gtest.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <unordered_map>
#include "adapters/http/withdraw/withdraw_controller.h"
#include "adapters/http/exception_handler.h"
#include "usecase/withdraw/withdraw_usecase.h"

namespace {

class FakeWithdrawSessionPort : public usecase::IWithdrawSessionPort {
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

class FakeWithdrawAccountPort : public usecase::IAccountingPort {
public:
    std::optional<double> find_balance_for_username(const std::string& username) const override {
        if (username == "ola") {
            return 250.0;
        }
        return std::nullopt;
    }

    void append_withdraw_for_username(const std::string& username, double amount) override {
        if (username == "ola") {
            current_balance_ = 250.0 - amount;
        }
    }

    double current_balance_{250.0};
};

}

class WithdrawControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto session_port = std::make_unique<FakeWithdrawSessionPort>();
        fake_session_port_ = session_port.get();

        usecase_ = std::make_unique<usecase::WithdrawUsecase>(
            std::move(session_port),
            std::make_unique<FakeWithdrawAccountPort>());
        controller_ = std::make_unique<adapters::http::WithdrawController>(*usecase_, exception_handler_);
    }

    FakeWithdrawSessionPort* fake_session_port_ = nullptr;
    std::unique_ptr<usecase::WithdrawUsecase> usecase_;
    adapters::http::ExceptionHandler exception_handler_;
    std::unique_ptr<adapters::http::WithdrawController> controller_;
};

TEST_F(WithdrawControllerTest, withdraw_without_session_token_is_rejected_with_authentication_error) {
    SCOPED_TRACE("Scenario 1.1: HTTP adapter — withdraw without token returns 401 Authentication error");

    httplib::Request req;
    req.body = R"({"amount":75.5})";
    httplib::Response res;

    controller_->handle_withdraw(req, res);

    EXPECT_EQ(res.status, 401);
    EXPECT_EQ(nlohmann::json::parse(res.body).value("error", ""), "Authentication error");
}

TEST_F(WithdrawControllerTest, authenticated_user_receives_the_updated_balance) {
    SCOPED_TRACE("Scenario 2.1: HTTP adapter — valid withdraw request returns the updated balance");

    fake_session_port_->given_active_session("ola.header.signature", "ola");

    httplib::Request req;
    req.set_header("Authorization", "Bearer ola.header.signature");
    req.body = R"({"amount":75.5})";
    httplib::Response res;

    controller_->handle_withdraw(req, res);

    EXPECT_EQ(res.status, 200);
    EXPECT_EQ(nlohmann::json::parse(res.body).value("balance", 0.0), 174.5);
}
