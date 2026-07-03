#pragma once
#include <string>

namespace usecase {

enum class AuthEventType {
    kUserCreated,
    kLogin,
    kLogout,
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

}