#include "adapters/http/auth/login_controller.h"
#include "domain/auth/login_request.h"
#include <nlohmann/json.hpp>

namespace adapters::http {

LoginController::LoginController(usecase::LoginUsecase& login_usecase,
                                 const ExceptionHandler& exception_handler)
    : login_usecase_(login_usecase)
    , exception_handler_(exception_handler) {}

void LoginController::handle_login(const httplib::Request& req, httplib::Response& res) {
    auto json = nlohmann::json::parse(req.body);
    std::string username = json.value("user", "");
    std::string pin = json.value("pin", "");

    exception_handler_.handle([&]() {
        login_usecase_.execute(domain::LoginRequest{username, pin});
    }, res);
}

}
