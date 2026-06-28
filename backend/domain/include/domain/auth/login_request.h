#pragma once
#include <string>
#include "domain/domain_exception.h"

namespace domain {

struct LoginRequest {
    const std::string username_;
    const std::string pin_;

    LoginRequest(std::string username, std::string pin);
};

}
