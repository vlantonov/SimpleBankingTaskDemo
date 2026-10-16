#pragma once
#include <string>
#include "domain/domain_exception.h"

namespace domain {

struct WithdrawRequest {
    const std::string token_;
    const double amount_;

    WithdrawRequest(std::string token, double amount);
};

}
