#include "server_configurator.h"
#include "adapters/http/auth/login_controller.h"
#include "adapters/http/exception_handler.h"
#include "usecase/auth/login_usecase.h"

static void handle_error(const httplib::Request&, httplib::Response& res) {
    if (res.body.empty()) {
        res.set_content(R"({"error": "Not Found"})", "application/json");
    }
}

static void configure_error_handler(httplib::Server& server) {
    server.set_error_handler(handle_error);
}

void configure_server(httplib::Server& server) {
    static usecase::LoginUsecase login_usecase;
    static adapters::http::ExceptionHandler exception_handler;
    static adapters::http::LoginController login_controller{login_usecase, exception_handler};

    server.Post("/api/v1/login", [](const httplib::Request& req, httplib::Response& res) {
        login_controller.handle_login(req, res);
    });
    configure_error_handler(server);
}
