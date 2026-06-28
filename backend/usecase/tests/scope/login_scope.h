#pragma once
#include <string>
#include "domain/auth/login_request.h"

struct LoginScope {
    std::string username{"ola"};
    std::string pin{"123"};

    domain::LoginRequest to_login_request() const {
        return domain::LoginRequest{username, pin};
    }

    struct Builder {
        std::string username_{"ola"};
        std::string pin_{"123"};

        Builder& with_username(std::string u) { username_ = std::move(u); return *this; }
        Builder& with_pin(std::string p) { pin_ = std::move(p); return *this; }
        LoginScope build() { return {username_, pin_}; }
    };
};
