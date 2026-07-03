#pragma once
#include <memory>
#include <optional>
#include <string>
#include "domain/auth/logout_request.h"
#include "usecase/auth/login_usecase.h"

namespace usecase {

class ILogoutSessionPort {
public:
    virtual ~ILogoutSessionPort() = default;
    virtual std::optional<std::string> find_username_by_token(const std::string& token) const = 0;
    virtual void close_session(const std::string& token) = 0;
};

class LogoutUsecase {
public:
    LogoutUsecase(std::unique_ptr<ILogoutSessionPort> session_port,
                  std::unique_ptr<IAuthEventLogPort> auth_event_log_port);

    void execute(const domain::LogoutRequest& request);

private:
    std::unique_ptr<ILogoutSessionPort> session_port_;
    std::unique_ptr<IAuthEventLogPort> auth_event_log_port_;
};

}