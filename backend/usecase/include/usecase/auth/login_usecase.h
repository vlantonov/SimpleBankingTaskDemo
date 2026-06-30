#pragma once
#include <memory>
#include <optional>
#include <string>
#include "domain/auth/login_request.h"
#include "usecase/auth/login_response.h"

namespace usecase {

struct AuthUser {
    std::string username;
    std::string pin;
};

class IUserPort {
public:
    virtual ~IUserPort() = default;
    virtual std::optional<AuthUser> find_by_username(const std::string& username) const = 0;
    virtual AuthUser create(const std::string& username, const std::string& pin) = 0;
};

class ISessionPort {
public:
    virtual ~ISessionPort() = default;
    virtual std::string open_session_for(const std::string& username) = 0;
};

enum class AuthEventType {
    kUserCreated,
    kLogin,
};

struct AuthEvent {
    AuthEventType type;
    std::string username;
};

class IAuthEventLogPort {
public:
    virtual ~IAuthEventLogPort() = default;
    virtual void append(const AuthEvent& event) = 0;
};

class LoginUsecase {
public:
    LoginUsecase();
    LoginUsecase(std::unique_ptr<IUserPort> user_port,
                 std::unique_ptr<ISessionPort> session_port,
                 std::unique_ptr<IAuthEventLogPort> auth_event_log_port);

    LoginResponse execute(const domain::LoginRequest& request);

private:
    AuthUser find_or_create_user(const domain::LoginRequest& request);
    static std::string welcome_message_for(const AuthUser& user);

    std::unique_ptr<IUserPort> user_port_;
    std::unique_ptr<ISessionPort> session_port_;
    std::unique_ptr<IAuthEventLogPort> auth_event_log_port_;
};

}
