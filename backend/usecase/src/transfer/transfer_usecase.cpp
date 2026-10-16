#include "usecase/transfer/transfer_usecase.h"

namespace usecase {

TransferUsecase::TransferUsecase(std::unique_ptr<ITransferSessionPort> session_port,
                                 std::unique_ptr<ITransferAccountPort> account_port)
    : session_port_(std::move(session_port))
    , account_port_(std::move(account_port)) {}

TransferResponse TransferUsecase::execute(const domain::TransferRequest& request) const {
    const auto source_username = session_port_->find_username_by_token(request.token()).value();
    const auto source_starting_balance = account_port_->find_balance_for_username(source_username).value();
    const auto destination_starting_balance = account_port_->find_balance_for_username(request.destination_username()).value();

    account_port_->append_transfer_between(source_username, request.destination_username(), request.amount());

    return TransferResponse{
        source_starting_balance - request.amount(),
        destination_starting_balance + request.amount()
    };
}

}
