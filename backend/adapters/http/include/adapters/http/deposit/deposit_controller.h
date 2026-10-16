#pragma once
#include <httplib.h>
#include "adapters/http/exception_handler.h"
#include "usecase/deposit/deposit_usecase.h"

namespace adapters::http {

class DepositController {
public:
    explicit DepositController(usecase::DepositUsecase& deposit_usecase,
                              const ExceptionHandler& exception_handler);

    void handle_deposit(const httplib::Request& req, httplib::Response& res) const;

private:
    usecase::DepositUsecase& deposit_usecase_;
    const ExceptionHandler& exception_handler_;
};

}
