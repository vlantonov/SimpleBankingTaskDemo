#include "usecase/withdraw/withdraw_usecase.h"
#include "domain/domain_exception.h"

namespace usecase {

WithdrawUsecase::WithdrawUsecase(std::unique_ptr<IWithdrawSessionPort> session_port,
                                 std::unique_ptr<IAccountingPort> account_port)
    : session_port_(std::move(session_port))
    , account_port_(std::move(account_port)) {}

WithdrawResponse WithdrawUsecase::execute(const domain::WithdrawRequest& request) const {
    const auto username = session_port_->find_username_by_token(request.token_);
    if (!username.has_value()) {
        throw domain::InvalidCredentialsException("Authentication error");
    }

    const auto balance = account_port_->find_balance_for_username(*username);
    if (!balance.has_value()) {
        throw domain::InvalidCredentialsException("Authentication error");
    }

    if (request.amount_ > *balance) {
        throw domain::ValidationException("Validation error");
    }

    const auto updated_balance = *balance - request.amount_;
    account_port_->append_withdraw_for_username(*username, request.amount_);
    return WithdrawResponse{updated_balance};
}

}
