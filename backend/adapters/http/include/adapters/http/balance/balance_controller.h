#pragma once
#include <httplib.h>
#include "adapters/http/exception_handler.h"
#include "usecase/balance/get_balance_usecase.h"

namespace adapters::http {

class BalanceController {
public:
    explicit BalanceController(usecase::GetBalanceUsecase& get_balance_usecase,
                              const ExceptionHandler& exception_handler);

    void handle_balance(const httplib::Request& req, httplib::Response& res) const;

private:
    usecase::GetBalanceUsecase& get_balance_usecase_;
    const ExceptionHandler& exception_handler_;
};

}
