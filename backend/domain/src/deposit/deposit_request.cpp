#include "domain/deposit/deposit_request.h"

namespace domain {

DepositRequest::DepositRequest(std::string token, double amount)
    : token_(std::move(token))
    , amount_(amount) {
    if (amount_ <= 0.0) {
        throw ValidationException("Validation error");
    }
}

}
