#include "usecase/deposit/deposit_usecase.h"
#include "domain/domain_exception.h"

namespace usecase {

DepositUsecase::DepositUsecase(std::unique_ptr<IDepositSessionPort> session_port,
                               std::unique_ptr<IDepositAccountingPort> account_port)
    : session_port_(std::move(session_port))
    , account_port_(std::move(account_port)) {}

DepositResponse DepositUsecase::execute(const domain::DepositRequest& request) const {
    const auto username = session_port_->find_username_by_token(request.token_);
    if (!username.has_value()) {
        throw domain::InvalidCredentialsException("Authentication error");
    }

    const auto balance = account_port_->find_balance_for_username(*username);
    if (!balance.has_value()) {
        throw domain::InvalidCredentialsException("Authentication error");
    }

    const auto updated_balance = *balance + request.amount_;
    account_port_->append_deposit_for_username(*username, request.amount_);
    return DepositResponse{updated_balance};
}

}
