#include "usecase/auth/logout_usecase.h"
#include <stdexcept>

namespace usecase {

LogoutUsecase::LogoutUsecase(std::unique_ptr<ILogoutSessionPort> session_port,
                             std::unique_ptr<IAuthEventLogPort> auth_event_log_port)
    : session_port_(std::move(session_port))
    , auth_event_log_port_(std::move(auth_event_log_port)) {}

void LogoutUsecase::execute(const domain::LogoutRequest&) {
    throw std::logic_error("Not implemented");
}

}