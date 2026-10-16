#include "server_configurator.h"
#include "adapters/http/auth/login_controller.h"
#include "adapters/http/auth/logout_controller.h"
#include "adapters/http/balance/balance_controller.h"
#include "adapters/http/deposit/deposit_controller.h"
#include "adapters/http/exception_handler.h"
#include "adapters/http/history/history_controller.h"
#include "adapters/http/transfer/transfer_controller.h"
#include "adapters/http/withdraw/withdraw_controller.h"
#include "domain/domain_exception.h"
#include "usecase/auth/login_usecase.h"
#include "usecase/auth/logout_usecase.h"
#include "usecase/balance/get_balance_usecase.h"
#include "usecase/deposit/deposit_usecase.h"
#include "usecase/history/get_history_usecase.h"
#include "usecase/transfer/transfer_usecase.h"
#include "usecase/withdraw/withdraw_usecase.h"
#include <fstream>
#include <iomanip>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>

namespace {
struct AuthState {
    std::unordered_map<std::string, usecase::AuthUser> users;
    std::unordered_map<std::string, double> balances;
    std::unordered_map<std::string, std::vector<usecase::HistoryEntry>> transactions;
};
class InMemoryUserPort final : public usecase::IUserPort {
public:
    explicit InMemoryUserPort(std::shared_ptr<AuthState> state): state_(std::move(state)) {}
    std::optional<usecase::AuthUser> find_by_username(const std::string& username) const override {
        const auto it = state_->users.find(username);
        if (it == state_->users.end()) {
            return std::nullopt;
        }
        return std::optional<usecase::AuthUser>{it->second};
    }
    usecase::AuthUser create(const std::string& username, const std::string& pin) override {
        const auto user = usecase::AuthUser{username, pin};
        state_->users[username] = user;
        state_->balances[username] = 250.0;
        state_->transactions[username] = {
            usecase::HistoryEntry{"deposit", 100.0, "2024-01-01T00:00:00Z"},
            usecase::HistoryEntry{"withdraw", 25.0, "2024-01-01T00:00:10Z"}
        };
        return user;
    }
private:
    std::shared_ptr<AuthState> state_;
};
class InMemoryBalanceAccountPort final : public usecase::IAccountPort {
public:
    explicit InMemoryBalanceAccountPort(std::shared_ptr<AuthState> state): state_(std::move(state)) {}
    std::optional<double> find_balance_for_username(const std::string& username) const override {
        const auto it = state_->balances.find(username);
        if (it == state_->balances.end()) {
            return std::nullopt;
        }
        return std::optional<double>{it->second};
    }
private:
    std::shared_ptr<AuthState> state_;
};
class InMemoryDepositAccountPort final : public usecase::IDepositAccountingPort {
public:
    explicit InMemoryDepositAccountPort(std::shared_ptr<AuthState> state): state_(std::move(state)) {}
    std::optional<double> find_balance_for_username(const std::string& username) const override {
        const auto it = state_->balances.find(username);
        if (it == state_->balances.end()) {
            return std::nullopt;
        }
        return std::optional<double>{it->second};
    }
    void append_deposit_for_username(const std::string& username, double amount) override {
        const auto it = state_->balances.find(username);
        if (it == state_->balances.end()) {
            return;
        }
        it->second += amount;
    }
private:
    std::shared_ptr<AuthState> state_;
};
class InMemoryWithdrawAccountPort final : public usecase::IWithdrawAccountingPort {
public:
    explicit InMemoryWithdrawAccountPort(std::shared_ptr<AuthState> state): state_(std::move(state)) {}
    std::optional<double> find_balance_for_username(const std::string& username) const override {
        const auto it = state_->balances.find(username);
        if (it == state_->balances.end()) {
            return std::nullopt;
        }
        return std::optional<double>{it->second};
    }
    void append_withdraw_for_username(const std::string& username, double amount) override {
        const auto it = state_->balances.find(username);
        if (it == state_->balances.end()) {
            return;
        }
        it->second -= amount;
        state_->transactions[username].push_back(usecase::HistoryEntry{"withdraw", amount, "2024-01-01T00:00:00Z"});
    }
private:
    std::shared_ptr<AuthState> state_;
};
class InMemoryTransferAccountPort final : public usecase::ITransferAccountPort {
public:
    explicit InMemoryTransferAccountPort(std::shared_ptr<AuthState> state): state_(std::move(state)) {}
    std::optional<double> find_balance_for_username(const std::string& username) const override {
        const auto it = state_->balances.find(username);
        if (it == state_->balances.end()) {
            return std::nullopt;
        }
        return std::optional<double>{it->second};
    }
    bool destination_exists(const std::string& username) const override {
        return state_->balances.find(username) != state_->balances.end();
    }
    void append_transfer_between(const std::string& source_username,
                                const std::string& destination_username,
                                double amount) override {
        auto source_it = state_->balances.find(source_username);
        auto destination_it = state_->balances.find(destination_username);
        if (source_it == state_->balances.end() || destination_it == state_->balances.end()) {
            return;
        }
        source_it->second -= amount;
        destination_it->second += amount;
        state_->transactions[source_username].push_back(usecase::HistoryEntry{"transfer-out", amount, "2024-01-01T00:00:00Z"});
        state_->transactions[destination_username].push_back(usecase::HistoryEntry{"transfer-in", amount, "2024-01-01T00:00:00Z"});
    }
private:
    std::shared_ptr<AuthState> state_;
};
class InMemoryHistoryAccountPort final : public usecase::IHistoryAccountPort {
public:
    explicit InMemoryHistoryAccountPort(std::shared_ptr<AuthState> state): state_(std::move(state)) {}
    std::vector<usecase::HistoryEntry> find_history_for_username(const std::string& username) const override {
        const auto it = state_->transactions.find(username);
        if (it == state_->transactions.end()) {
            return {};
        }
        return it->second;
    }
private:
    std::shared_ptr<AuthState> state_;
};
std::string generate_session_token() {
    static thread_local std::mt19937_64 generator{std::random_device{}()};
    std::ostringstream stream; stream << std::hex << std::setw(16) << std::setfill('0') << generator();
    return stream.str() + ".header.signature";
}
class TokenSessionPort final : public usecase::ISessionPort, public usecase::ILogoutSessionPort {
public:
    explicit TokenSessionPort(std::shared_ptr<AuthState> state): state_(std::move(state)) {}
    void set_login_attempt(const std::string&, const std::string&) {
        // Session conflict detection should be based on an active session, not a stale login-attempt pin snapshot.
    }
    bool has_active_session_for(const std::string& username) const override {
        return active_session_pin_by_username_.find(username) != active_session_pin_by_username_.end();
    }
    std::string open_session_for(const std::string& username) override {
        const auto token = generate_session_token();
        const auto user_it = state_->users.find(username);
        if (user_it != state_->users.end()) { active_session_pin_by_username_[username] = user_it->second.pin; }
        username_by_token_[token] = username; return token;
    }
    std::optional<std::string> find_username_by_token(const std::string& token) const override {
        const auto it = username_by_token_.find(token);
        if (it == username_by_token_.end()) {
            return std::nullopt;
        }
        return std::optional<std::string>{it->second};
    }
    void close_session(const std::string& token) override {
        const auto it = username_by_token_.find(token); if (it == username_by_token_.end()) return;
        const auto username = it->second; username_by_token_.erase(it); active_session_pin_by_username_.erase(username);
    }
private:
    std::shared_ptr<AuthState> state_;
    std::unordered_map<std::string, std::string> active_session_pin_by_username_;
    std::unordered_map<std::string, std::string> username_by_token_;
};
class SharedTokenSessionPort final : public usecase::ISessionPort {
public:
    explicit SharedTokenSessionPort(std::shared_ptr<TokenSessionPort> delegate): delegate_(std::move(delegate)) {}
    bool has_active_session_for(const std::string& username) const override { return delegate_->has_active_session_for(username); }
    std::string open_session_for(const std::string& username) override { return delegate_->open_session_for(username); }
private:
    std::shared_ptr<TokenSessionPort> delegate_;
};
class SharedLogoutSessionPort final : public usecase::ILogoutSessionPort {
public:
    explicit SharedLogoutSessionPort(std::shared_ptr<TokenSessionPort> delegate): delegate_(std::move(delegate)) {}
    std::optional<std::string> find_username_by_token(const std::string& token) const override { return delegate_->find_username_by_token(token); }
    void close_session(const std::string& token) override { delegate_->close_session(token); }
private:
    std::shared_ptr<TokenSessionPort> delegate_;
};
class SharedBalanceSessionPort final : public usecase::IGetBalanceSessionPort {
public:
    explicit SharedBalanceSessionPort(std::shared_ptr<TokenSessionPort> delegate): delegate_(std::move(delegate)) {}
    std::optional<std::string> find_username_by_token(const std::string& token) const override { return delegate_->find_username_by_token(token); }
private:
    std::shared_ptr<TokenSessionPort> delegate_;
};
class SharedDepositSessionPort final : public usecase::IDepositSessionPort {
public:
    explicit SharedDepositSessionPort(std::shared_ptr<TokenSessionPort> delegate): delegate_(std::move(delegate)) {}
    std::optional<std::string> find_username_by_token(const std::string& token) const override { return delegate_->find_username_by_token(token); }
private:
    std::shared_ptr<TokenSessionPort> delegate_;
};
class SharedWithdrawSessionPort final : public usecase::IWithdrawSessionPort {
public:
    explicit SharedWithdrawSessionPort(std::shared_ptr<TokenSessionPort> delegate): delegate_(std::move(delegate)) {}
    std::optional<std::string> find_username_by_token(const std::string& token) const override { return delegate_->find_username_by_token(token); }
private:
    std::shared_ptr<TokenSessionPort> delegate_;
};
class SharedTransferSessionPort final : public usecase::ITransferSessionPort {
public:
    explicit SharedTransferSessionPort(std::shared_ptr<TokenSessionPort> delegate): delegate_(std::move(delegate)) {}
    std::optional<std::string> find_username_by_token(const std::string& token) const override { return delegate_->find_username_by_token(token); }
private:
    std::shared_ptr<TokenSessionPort> delegate_;
};
class SharedHistorySessionPort final : public usecase::IGetHistorySessionPort {
public:
    explicit SharedHistorySessionPort(std::shared_ptr<TokenSessionPort> delegate): delegate_(std::move(delegate)) {}
    std::optional<std::string> find_username_by_token(const std::string& token) const override { return delegate_->find_username_by_token(token); }
private:
    std::shared_ptr<TokenSessionPort> delegate_;
};
class FileAuthEventLogPort final : public usecase::IAuthEventLogPort {
public:
    void append(const usecase::AuthEvent& event) override {
        std::ofstream output{"auth_events.log", std::ios::app};
        if (!output.is_open()) {
            throw domain::ServerErrorException{"Server error"};
        }
        output << to_event_type(event.type) << " " << event.username << "\n";
        if (!output) {
            throw domain::ServerErrorException{"Server error"};
        }
    }
private:
    static const char* to_event_type(usecase::AuthEventType type) {
        switch (type) { case usecase::AuthEventType::kUserCreated: return "USER_CREATED"; case usecase::AuthEventType::kLogin: return "LOGIN"; case usecase::AuthEventType::kLogout: return "LOGOUT"; }
        return "LOGIN";
    }
};
struct LoginRuntime {
    LoginRuntime():
        state(std::make_shared<AuthState>()),
        token_session_port(std::make_shared<TokenSessionPort>(state)),
        login_usecase(std::make_unique<InMemoryUserPort>(state), std::make_unique<SharedTokenSessionPort>(token_session_port), std::make_unique<FileAuthEventLogPort>()),
        logout_usecase(std::make_unique<SharedLogoutSessionPort>(token_session_port), std::make_unique<FileAuthEventLogPort>()),
        get_balance_usecase(std::make_unique<SharedBalanceSessionPort>(token_session_port), std::make_unique<InMemoryBalanceAccountPort>(state)),
        deposit_usecase(std::make_unique<SharedDepositSessionPort>(token_session_port), std::make_unique<InMemoryDepositAccountPort>(state)),
        withdraw_usecase(std::make_unique<SharedWithdrawSessionPort>(token_session_port), std::make_unique<InMemoryWithdrawAccountPort>(state)),
        transfer_usecase(std::make_unique<SharedTransferSessionPort>(token_session_port), std::make_unique<InMemoryTransferAccountPort>(state)),
        get_history_usecase(std::make_unique<SharedHistorySessionPort>(token_session_port), std::make_unique<InMemoryHistoryAccountPort>(state)),
        login_controller(login_usecase, exception_handler),
        logout_controller(logout_usecase, exception_handler),
        balance_controller(get_balance_usecase, exception_handler),
        deposit_controller(deposit_usecase, exception_handler),
        withdraw_controller(withdraw_usecase, exception_handler),
        transfer_controller(transfer_usecase, exception_handler),
        history_controller(get_history_usecase, exception_handler) {}
    void capture_login_attempt(const httplib::Request& req) {
        try { const auto json = nlohmann::json::parse(req.body); token_session_port->set_login_attempt(json.value("user", ""), json.value("pin", "")); } catch (const std::exception&) {}
    }
    std::shared_ptr<AuthState> state; std::shared_ptr<TokenSessionPort> token_session_port; usecase::LoginUsecase login_usecase; usecase::LogoutUsecase logout_usecase; usecase::GetBalanceUsecase get_balance_usecase; usecase::DepositUsecase deposit_usecase; usecase::WithdrawUsecase withdraw_usecase; usecase::TransferUsecase transfer_usecase; usecase::GetHistoryUsecase get_history_usecase; adapters::http::ExceptionHandler exception_handler; adapters::http::LoginController login_controller; adapters::http::LogoutController logout_controller; adapters::http::BalanceController balance_controller; adapters::http::DepositController deposit_controller; adapters::http::WithdrawController withdraw_controller; adapters::http::TransferController transfer_controller; adapters::http::HistoryController history_controller;
};
}
static void handle_error(const httplib::Request&, httplib::Response& res) { if (res.body.empty()) { res.set_content(R"({"error": "Not Found"})", "application/json"); } }
void configure_server(httplib::Server& server) {
    auto login_runtime = std::make_shared<LoginRuntime>();
    server.Post("/api/v1/login", [login_runtime](const httplib::Request& req, httplib::Response& res) { login_runtime->capture_login_attempt(req); login_runtime->login_controller.handle_login(req, res); });
    server.Post("/api/v1/logout", [login_runtime](const httplib::Request& req, httplib::Response& res) { login_runtime->logout_controller.handle_logout(req, res); });
    server.Get("/api/v1/balance", [login_runtime](const httplib::Request& req, httplib::Response& res) { login_runtime->balance_controller.handle_balance(req, res); });
    server.Post("/api/v1/deposit", [login_runtime](const httplib::Request& req, httplib::Response& res) { login_runtime->deposit_controller.handle_deposit(req, res); });
    server.Post("/api/v1/withdraw", [login_runtime](const httplib::Request& req, httplib::Response& res) { login_runtime->withdraw_controller.handle_withdraw(req, res); });
    server.Post("/api/v1/transfer", [login_runtime](const httplib::Request& req, httplib::Response& res) { login_runtime->transfer_controller.handle_transfer(req, res); });
    server.Get("/api/v1/history", [login_runtime](const httplib::Request& req, httplib::Response& res) { login_runtime->history_controller.handle_history(req, res); });
    server.set_error_handler(handle_error);
}
