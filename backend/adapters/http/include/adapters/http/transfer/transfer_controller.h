#pragma once
#include <httplib.h>
#include "adapters/http/exception_handler.h"
#include "usecase/transfer/transfer_usecase.h"

namespace adapters::http {

class TransferController {
public:
    explicit TransferController(usecase::TransferUsecase& transfer_usecase,
                                const ExceptionHandler& exception_handler);

    void handle_transfer(const httplib::Request& req, httplib::Response& res) const;

private:
    usecase::TransferUsecase& transfer_usecase_;
    const ExceptionHandler& exception_handler_;
};

}
