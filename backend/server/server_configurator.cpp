#include "server_configurator.h"
#include "adapters/http/auth/login_controller.h"
#include "adapters/http/exception_handler.h"
#include "usecase/auth/login_usecase.h"
#include <memory>
#include <optional>

namespace {

class InMemoryUserPort final : public usecase::IUserPort {
public:
    std::optional<usecase::AuthUser> find_by_username(const std::string&) const override {
        return std::nullopt;
    }

    usecase::AuthUser create(const std::string& username, const std::string& pin) override {
        return usecase::AuthUser{username, pin};
    }
};

class TokenSessionPort final : public usecase::ISessionPort {
public:
    std::string open_session_for(const std::string& username) override {
        return username + ".header.signature";
    }
};

usecase::LoginUsecase create_login_usecase() {
    return usecase::LoginUsecase(
        std::make_unique<InMemoryUserPort>(),
        std::make_unique<TokenSessionPort>()
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
