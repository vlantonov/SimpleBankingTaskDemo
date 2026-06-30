#include "usecase/auth/login_usecase.h"

namespace usecase {

namespace {

class FirstLoginUserPort final : public IUserPort {
public:
    std::optional<AuthUser> find_by_username(const std::string&) const override {
        return std::nullopt;
    }

    AuthUser create(const std::string& username, const std::string& pin) override {
        return AuthUser{username, pin};
    }
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
    const auto found_user = user_port_->find_by_username(request.username_);
    if (found_user) {
        return *found_user;
    }
    const auto created_user = user_port_->create(request.username_, request.pin_);
    auth_event_log_port_->append(AuthEvent{AuthEventType::kUserCreated, created_user.username});
    return created_user;
}

std::string LoginUsecase::welcome_message_for(const AuthUser& user) {
    return "Welcome, " + user.username;
}

LoginResponse LoginUsecase::execute(const domain::LoginRequest& request) {
    const auto user = find_or_create_user(request);
    const auto token = session_port_->open_session_for(user.username);
    auth_event_log_port_->append(AuthEvent{AuthEventType::kLogin, user.username});
    return LoginResponse{welcome_message_for(user), token};
}

}
