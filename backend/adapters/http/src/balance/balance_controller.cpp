#include "adapters/http/balance/balance_controller.h"
#include <stdexcept>

namespace adapters::http {

BalanceController::BalanceController(usecase::GetBalanceUsecase& get_balance_usecase,
                                     const ExceptionHandler& exception_handler)
    : get_balance_usecase_(get_balance_usecase)
    , exception_handler_(exception_handler) {}

void BalanceController::handle_balance(const httplib::Request& req, httplib::Response& res) const {
    exception_handler_.handle([&]() {
        // Red phase scaffold only: missing HTTP adapter logic intentionally fails the test.
        throw std::runtime_error("not implemented");
    }, res);
}

}
