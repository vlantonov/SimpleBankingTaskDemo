#pragma once
#include <httplib.h>
#include "usecase/auth/login_usecase.h"
#include "adapters/http/exception_handler.h"

namespace adapters::http {

class LoginController {
public:
    explicit LoginController(usecase::LoginUsecase& login_usecase,
                             const ExceptionHandler& exception_handler);

    void handle_login(const httplib::Request& req, httplib::Response& res);

private:
    usecase::LoginUsecase& login_usecase_;
    const ExceptionHandler& exception_handler_;
};

}
