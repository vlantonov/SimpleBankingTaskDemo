#include "usecase/transfer/transfer_usecase.h"

#include "domain/domain_exception.h"

namespace usecase {

TransferUsecase::TransferUsecase(std::unique_ptr<ITransferSessionPort> session_port,
                                 std::unique_ptr<ITransferAccountPort> account_port)
    : session_port_(std::move(session_port))
    , account_port_(std::move(account_port)) {}

std::string TransferUsecase::require_source_username(const domain::TransferRequest& request) const {
    const auto source_username = session_port_->find_username_by_token(request.token());
    if (!source_username.has_value()) {
        throw domain::InvalidCredentialsException("Authentication error");
    }

    return source_username.value();
}

std::string TransferUsecase::require_destination_username(const domain::TransferRequest& request) const {
    if (!account_port_->destination_exists(request.destination_username())) {
        throw domain::ValidationException("Destination account not found");
    }

    return request.destination_username();
}

double TransferUsecase::find_starting_balance(const std::string& username) const {
    return account_port_->find_balance_for_username(username).value();
}

void TransferUsecase::append_transfer(const std::string& source_username,
                                      const std::string& destination_username,
                                      double amount) const {
    account_port_->append_transfer_between(source_username, destination_username, amount);
}

TransferResponse TransferUsecase::build_transfer_response(double source_starting_balance,
                                                         double destination_starting_balance,
                                                         double amount) const {
    return TransferResponse{
        source_starting_balance - amount,
        destination_starting_balance + amount
    };
}

TransferResponse TransferUsecase::execute(const domain::TransferRequest& request) const {
    const auto source_username = require_source_username(request);
    const auto source_starting_balance = find_starting_balance(source_username);
    const auto destination_username = require_destination_username(request);
    const auto destination_starting_balance = find_starting_balance(destination_username);

    append_transfer(source_username, destination_username, request.amount());
    return build_transfer_response(source_starting_balance, destination_starting_balance, request.amount());
}

}
