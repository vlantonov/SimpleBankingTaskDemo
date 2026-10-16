#pragma once
#include <string>
#include "domain/domain_exception.h"

namespace domain {

struct DepositRequest {
    const std::string token_;
    const double amount_;

    DepositRequest(std::string token, double amount);
};

}
