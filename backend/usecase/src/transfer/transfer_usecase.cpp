#include "usecase/transfer/transfer_usecase.h"
#include <stdexcept>

namespace usecase {

TransferUsecase::TransferUsecase(std::unique_ptr<ITransferSessionPort> session_port,
                                 std::unique_ptr<ITransferAccountPort> account_port)
    : session_port_(std::move(session_port))
    , account_port_(std::move(account_port)) {}

TransferResponse TransferUsecase::execute(const domain::TransferRequest&) const {
    throw std::logic_error("Not implemented");
}

}
