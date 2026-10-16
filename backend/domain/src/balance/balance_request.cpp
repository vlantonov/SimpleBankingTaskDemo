#include "domain/balance/balance_request.h"

namespace domain {

BalanceRequest::BalanceRequest(std::string token)
    : token_(std::move(token)) {}

}
