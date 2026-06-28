#pragma once
#include <string>
#include "domain/domain_exception.h"

namespace domain {

struct LoginRequest {
    std::string username;
    std::string pin;

    LoginRequest(std::string username, std::string pin);
};

}
