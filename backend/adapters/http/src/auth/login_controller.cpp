#include "adapters/http/auth/login_controller.h"
#include <stdexcept>

namespace adapters::http {

LoginController::LoginController(usecase::LoginUsecase& login_usecase,
                                 const ExceptionHandler& exception_handler)
    : login_usecase_(login_usecase)
    , exception_handler_(exception_handler) {}

void LoginController::handle_login(const httplib::Request& req, httplib::Response& res) {
    throw std::logic_error("Not implemented");
}

}
