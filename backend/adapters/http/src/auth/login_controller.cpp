#include "adapters/http/auth/login_controller.h"
#include "domain/auth/login_request.h"
#include <nlohmann/json.hpp>

namespace adapters::http {

LoginController::LoginController(usecase::LoginUsecase& login_usecase,
                                 const ExceptionHandler& exception_handler)
    : login_usecase_(login_usecase)
    , exception_handler_(exception_handler) {}

void LoginController::handle_login(const httplib::Request& req, httplib::Response& res) const {
    exception_handler_.handle([&]() {
        const auto json = nlohmann::json::parse(req.body);
        login_usecase_.execute(domain::LoginRequest{
            json.value("user", ""),
            json.value("pin", "")
        });
    }, res);
}

}
