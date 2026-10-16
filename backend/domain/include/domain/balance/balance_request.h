#pragma once
#include <string>

namespace domain {

struct BalanceRequest {
    const std::string token_;

    explicit BalanceRequest(std::string token);
};

}
