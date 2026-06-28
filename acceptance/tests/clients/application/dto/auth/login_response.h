#pragma once
#include <string>

namespace dto::auth {

struct LoginResponse {
    std::string message;
    std::string token;
    std::string error;
    int status_code{};
};

}
