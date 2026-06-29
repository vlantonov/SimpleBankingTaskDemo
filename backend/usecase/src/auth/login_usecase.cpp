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

}  // namespace

LoginUsecase::LoginUsecase()
    : LoginUsecase(std::make_unique<FirstLoginUserPort>(), std::make_unique<TokenSessionPort>()) {}

LoginUsecase::LoginUsecase(std::unique_ptr<IUserPort> user_port,
                           std::unique_ptr<ISessionPort> session_port)
    : user_port_(std::move(user_port))
    , session_port_(std::move(session_port)) {}

domain::LoginRequest LoginUsecase::validated_copy_of(const domain::LoginRequest& request) {
    return domain::LoginRequest{request.username_, request.pin_};
}

AuthUser LoginUsecase::find_or_create_user(const domain::LoginRequest& validated_request) {
    const auto found_user = user_port_->find_by_username(validated_request.username_);
    if (found_user) {
        return *found_user;
    }
    return user_port_->create(validated_request.username_, validated_request.pin_);
}

std::string LoginUsecase::welcome_message_for(const AuthUser& user) {
    return "Welcome, " + user.username;
}

LoginResponse LoginUsecase::execute(const domain::LoginRequest& request) {
    const auto validated_request = validated_copy_of(request);
    const auto user = find_or_create_user(validated_request);
    const auto token = session_port_->open_session_for(user.username);
    return LoginResponse{welcome_message_for(user), token};
}

}
