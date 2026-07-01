#include "usecase/auth/login_usecase.h"
#include "domain/domain_exception.h"
#include <unordered_map>

namespace usecase {

namespace {

class FirstLoginUserPort final : public IUserPort {
public:
    std::optional<AuthUser> find_by_username(const std::string& username) const override {
        const auto it = users_.find(username);
        if (it == users_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    AuthUser create(const std::string& username, const std::string& pin) override {
        const auto user = AuthUser{username, pin};
        users_[username] = user;
        return user;
    }

private:
    mutable std::unordered_map<std::string, AuthUser> users_;
};

class TokenSessionPort final : public ISessionPort {
public:
    std::string open_session_for(const std::string& username) override {
        return username + ".header.signature";
    }
};

class NoOpAuthEventLogPort final : public IAuthEventLogPort {
public:
    void append(const AuthEvent&) override {}
};

}  // namespace

LoginUsecase::LoginUsecase()
    : LoginUsecase(
        std::make_unique<FirstLoginUserPort>(),
        std::make_unique<TokenSessionPort>(),
        std::make_unique<NoOpAuthEventLogPort>()
    ) {}

LoginUsecase::LoginUsecase(std::unique_ptr<IUserPort> user_port,
                           std::unique_ptr<ISessionPort> session_port,
                           std::unique_ptr<IAuthEventLogPort> auth_event_log_port)
    : user_port_(std::move(user_port))
    , session_port_(std::move(session_port))
    , auth_event_log_port_(std::move(auth_event_log_port)) {}

AuthUser LoginUsecase::find_or_create_user(const domain::LoginRequest& request) {
    if (const auto found_user = user_port_->find_by_username(request.username_)) {
        return validate_user_credentials(*found_user, request.pin_);
    }
    return create_user_and_log_event(request);
}

AuthUser LoginUsecase::validate_user_credentials(const AuthUser& user, const std::string& pin) {
    if (user.pin != pin) {
        throw domain::InvalidCredentialsException("Invalid credentials");
    }
    return user;
}

AuthUser LoginUsecase::create_user_and_log_event(const domain::LoginRequest& request) {
    const auto created_user = user_port_->create(request.username_, request.pin_);
    append_auth_event(AuthEventType::kUserCreated, created_user.username);
    return created_user;
}

void LoginUsecase::append_auth_event(AuthEventType type, const std::string& username) {
    auth_event_log_port_->append(AuthEvent{type, username});
}

std::string LoginUsecase::welcome_message_for(const AuthUser& user) {
    return "Welcome, " + user.username;
}

LoginResponse LoginUsecase::execute(const domain::LoginRequest& request) {
    const auto user = find_or_create_user(request);
    const auto token = session_port_->open_session_for(user.username);
    append_auth_event(AuthEventType::kLogin, user.username);
    return LoginResponse{welcome_message_for(user), token};
}

}
