#include "usecase/auth/logout_usecase.h"
#include "domain/domain_exception.h"

namespace usecase {

LogoutUsecase::LogoutUsecase(std::unique_ptr<ILogoutSessionPort> session_port,
                             std::unique_ptr<IAuthEventLogPort> auth_event_log_port)
    : session_port_(std::move(session_port))
    , auth_event_log_port_(std::move(auth_event_log_port)) {}

void LogoutUsecase::execute(const domain::LogoutRequest& request) {
    const auto username = session_port_->find_username_by_token(request.token_);
    if (!username.has_value()) {
        throw domain::InvalidCredentialsException("Authentication error");
    }
    session_port_->close_session(request.token_);
    auth_event_log_port_->append(AuthEvent{AuthEventType::kLogout, *username});
}

}