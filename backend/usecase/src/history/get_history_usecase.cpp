#include "usecase/history/get_history_usecase.h"
#include "domain/domain_exception.h"

namespace usecase {

GetHistoryUsecase::GetHistoryUsecase(std::unique_ptr<IGetHistorySessionPort> session_port,
                                     std::unique_ptr<IHistoryAccountPort> account_port)
    : session_port_(std::move(session_port))
    , account_port_(std::move(account_port)) {}

HistoryResponse GetHistoryUsecase::execute(const domain::HistoryRequest& request) const {
    const auto username = session_port_->find_username_by_token(request.token_);
    if (!username.has_value()) {
        throw domain::InvalidCredentialsException("Authentication error");
    }

    return HistoryResponse{account_port_->find_history_for_username(*username)};
}

}
