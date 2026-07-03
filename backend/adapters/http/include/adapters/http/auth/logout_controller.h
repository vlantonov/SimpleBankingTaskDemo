#pragma once
#include <httplib.h>
#include "adapters/http/exception_handler.h"
#include "usecase/auth/logout_usecase.h"

namespace adapters::http {

class LogoutController {
public:
    explicit LogoutController(usecase::LogoutUsecase& logout_usecase,
                              const ExceptionHandler& exception_handler);

    void handle_logout(const httplib::Request& req, httplib::Response& res) const;

private:
    usecase::LogoutUsecase& logout_usecase_;
    const ExceptionHandler& exception_handler_;
};

}
