#include "server_configurator.h"
#include "adapters/http/auth/login_controller.h"
#include "adapters/http/auth/logout_controller.h"
#include "adapters/http/balance/balance_controller.h"
#include "adapters/http/deposit/deposit_controller.h"
#include "adapters/http/exception_handler.h"
#include "domain/domain_exception.h"
#include "usecase/auth/login_usecase.h"
#include "usecase/auth/logout_usecase.h"
#include "usecase/balance/get_balance_usecase.h"
#include "usecase/deposit/deposit_usecase.h"
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
class InMemoryDepositAccountPort final : public usecase::IAccountingPort {
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
        login_controller(login_usecase, exception_handler),
        logout_controller(logout_usecase, exception_handler),
        balance_controller(get_balance_usecase, exception_handler),
        deposit_controller(deposit_usecase, exception_handler) {}
    void capture_login_attempt(const httplib::Request& req) {
        try { const auto json = nlohmann::json::parse(req.body); token_session_port->set_login_attempt(json.value("user", ""), json.value("pin", "")); } catch (const std::exception&) {}
    }
    std::shared_ptr<AuthState> state; std::shared_ptr<TokenSessionPort> token_session_port; usecase::LoginUsecase login_usecase; usecase::LogoutUsecase logout_usecase; usecase::GetBalanceUsecase get_balance_usecase; usecase::DepositUsecase deposit_usecase; adapters::http::ExceptionHandler exception_handler; adapters::http::LoginController login_controller; adapters::http::LogoutController logout_controller; adapters::http::BalanceController balance_controller; adapters::http::DepositController deposit_controller;
};
}
static void handle_error(const httplib::Request&, httplib::Response& res) { if (res.body.empty()) { res.set_content(R"({"error": "Not Found"})", "application/json"); } }
void configure_server(httplib::Server& server) {
    auto login_runtime = std::make_shared<LoginRuntime>();
    server.Post("/api/v1/login", [login_runtime](const httplib::Request& req, httplib::Response& res) { login_runtime->capture_login_attempt(req); login_runtime->login_controller.handle_login(req, res); });
    server.Post("/api/v1/logout", [login_runtime](const httplib::Request& req, httplib::Response& res) { login_runtime->logout_controller.handle_logout(req, res); });
    server.Get("/api/v1/balance", [login_runtime](const httplib::Request& req, httplib::Response& res) { login_runtime->balance_controller.handle_balance(req, res); });
    server.Post("/api/v1/deposit", [login_runtime](const httplib::Request& req, httplib::Response& res) { login_runtime->deposit_controller.handle_deposit(req, res); });
    server.set_error_handler(handle_error);
}
