#pragma once
#include <string>
#include "domain/domain_exception.h"

namespace domain {

struct LogoutRequest {
    const std::string token_;

    explicit LogoutRequest(std::string token);
};

}