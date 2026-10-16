#include "adapters/http/transfer/transfer_controller.h"
#include <stdexcept>

namespace adapters::http {

TransferController::TransferController(usecase::TransferUsecase& transfer_usecase,
                                       const ExceptionHandler& exception_handler)
    : transfer_usecase_(transfer_usecase)
    , exception_handler_(exception_handler) {}

void TransferController::handle_transfer(const httplib::Request& req, httplib::Response& res) const {
    exception_handler_.handle([&]() {
        throw std::logic_error("Not implemented");
    }, res);
}

}
