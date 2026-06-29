#pragma once
#include <string>

namespace usecase {

struct LoginResponse {
    std::string message;
    std::string token;
};

}