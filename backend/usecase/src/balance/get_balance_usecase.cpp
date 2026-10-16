#include "usecase/balance/get_balance_usecase.h"
#include "domain/domain_exception.h"

namespace usecase {

GetBalanceUsecase::GetBalanceUsecase(std::unique_ptr<IGetBalanceSessionPort> session_port,
                                     std::unique_ptr<IAccountPort> account_port)
    : session_port_(std::move(session_port))
    , account_port_(std::move(account_port)) {}

BalanceResponse GetBalanceUsecase::execute(const domain::BalanceRequest& request) const {
    const auto username = session_port_->find_username_by_token(request.token_);
    if (!username.has_value()) {
        throw domain::InvalidCredentialsException("Authentication error");
    }

    const auto balance = account_port_->find_balance_for_username(*username);
    if (!balance.has_value()) {
        throw domain::InvalidCredentialsException("Authentication error");
    }

    return BalanceResponse{*balance};
}

}
