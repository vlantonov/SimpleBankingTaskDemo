#pragma once
#include <httplib.h>
#include "adapters/http/exception_handler.h"
#include "usecase/withdraw/withdraw_usecase.h"

namespace adapters::http {

class WithdrawController {
public:
    explicit WithdrawController(usecase::WithdrawUsecase& withdraw_usecase,
                               const ExceptionHandler& exception_handler);

    void handle_withdraw(const httplib::Request& req, httplib::Response& res) const;

private:
    usecase::WithdrawUsecase& withdraw_usecase_;
    const ExceptionHandler& exception_handler_;
};

}
