#include <gtest/gtest.h>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include "adapters/http/exception_handler.h"
#include "adapters/http/history/history_controller.h"
#include "statements/history_controller_statements.h"
#include "usecase/history/get_history_usecase.h"

namespace {

class FakeHistorySessionPort : public usecase::IGetHistorySessionPort {
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

class FakeHistoryAccountPort : public usecase::IHistoryAccountPort {
public:
    std::vector<usecase::HistoryEntry> find_history_for_username(const std::string& username) const override {
        if (username == "ola") {
            return {
                usecase::HistoryEntry{"deposit", 100.0, "2024-01-01T00:00:00Z"},
                usecase::HistoryEntry{"withdraw", 25.0, "2024-01-01T00:00:10Z"}
            };
        }
        return {};
    }
};

}

class HistoryControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        initialize_history_usecase();
        controller_ = std::make_unique<adapters::http::HistoryController>(*usecase_, exception_handler_);
        statements_ = std::make_unique<HistoryControllerStatements>(*controller_, active_session_initializer());
    }

    void initialize_history_usecase() {
        auto session_port = std::make_unique<FakeHistorySessionPort>();
        fake_history_session_port_ = session_port.get();

        usecase_ = std::make_unique<usecase::GetHistoryUsecase>(
            std::move(session_port),
            std::make_unique<FakeHistoryAccountPort>());
    }

    std::function<void(const std::string&, const std::string&)> active_session_initializer() {
        return [this](const std::string& token, const std::string& username) {
            fake_history_session_port_->given_active_session(token, username);
        };
    }

    FakeHistorySessionPort* fake_history_session_port_ = nullptr;
    std::unique_ptr<usecase::GetHistoryUsecase> usecase_;
    adapters::http::ExceptionHandler exception_handler_;
    std::unique_ptr<adapters::http::HistoryController> controller_;
    std::unique_ptr<HistoryControllerStatements> statements_;
};

TEST_F(HistoryControllerTest, history_without_session_token_is_rejected_with_authentication_error) {
    SCOPED_TRACE("Scenario 1.1: HTTP adapter — history without token returns 401 Authentication error");

    statements_->when_history_requested_without_authorization();
    statements_->assert_authentication_error_response();
}

TEST_F(HistoryControllerTest, authenticated_user_receives_their_transaction_history) {
    SCOPED_TRACE("Scenario 2.1: HTTP adapter — valid history request returns the transaction list");

    statements_->given_active_session("ola.header.signature", "ola");
    statements_->when_history_requested_with_authorization("Bearer ola.header.signature");
    statements_->assert_history_response({
        {"deposit", 100.0, "2024-01-01T00:00:00Z"},
        {"withdraw", 25.0, "2024-01-01T00:00:10Z"}
    });
}
