#include "server_configurator.h"
#include "adapters/http/auth/login_controller.h"
#include "adapters/http/exception_handler.h"
#include "usecase/auth/login_usecase.h"
#include <fstream>
#include <memory>
#include <optional>
#include <unordered_map>

namespace {

class InMemoryUserPort final : public usecase::IUserPort {
public:
    std::optional<usecase::AuthUser> find_by_username(const std::string& username) const override {
        const auto it = users_.find(username);
        if (it == users_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    usecase::AuthUser create(const std::string& username, const std::string& pin) override {
        const auto user = usecase::AuthUser{username, pin};
        users_[username] = user;
        return user;
    }

private:
    mutable std::unordered_map<std::string, usecase::AuthUser> users_;
};

class TokenSessionPort final : public usecase::ISessionPort {
public:
    std::string open_session_for(const std::string& username) override {
        return username + ".header.signature";
    }
};

class FileAuthEventLogPort final : public usecase::IAuthEventLogPort {
public:
    void append(const usecase::AuthEvent& event) override {
        std::ofstream output{"auth_events.log", std::ios::app};
        output << to_event_type(event.type) << " " << event.username << "\n";
    }

private:
    static const char* to_event_type(usecase::AuthEventType type) {
        switch (type) {
            case usecase::AuthEventType::kUserCreated:
                return "USER_CREATED";
            case usecase::AuthEventType::kLogin:
                return "LOGIN";
        }
        return "LOGIN";
    }
};

usecase::LoginUsecase create_login_usecase() {
    return usecase::LoginUsecase(
        std::make_unique<InMemoryUserPort>(),
        std::make_unique<TokenSessionPort>(),
        std::make_unique<FileAuthEventLogPort>()
    );
}

}

static void handle_error(const httplib::Request&, httplib::Response& res) {
    if (res.body.empty()) {
        res.set_content(R"({"error": "Not Found"})", "application/json");
    }
}

void configure_server(httplib::Server& server) {
    static usecase::LoginUsecase login_usecase = create_login_usecase();
    static adapters::http::ExceptionHandler exception_handler;
    static adapters::http::LoginController login_controller{login_usecase, exception_handler};

    server.Post("/api/v1/login", [](const httplib::Request& req, httplib::Response& res) {
        login_controller.handle_login(req, res);
    });
    server.set_error_handler(handle_error);
}
