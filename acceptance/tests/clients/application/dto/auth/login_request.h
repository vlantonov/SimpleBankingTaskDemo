#pragma once
#include <string>

namespace dto::auth {

struct LoginRequest {
    std::string user;
    std::string pin;
};

}
