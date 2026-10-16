#include "domain/auth/login_request.h"

namespace domain {

LoginRequest::LoginRequest(std::string username, std::string pin)
    : username_(std::move(username))
    , pin_(std::move(pin)) {
    if (username_.empty() || username_.size() > 256 || pin_.empty() || pin_.size() > 64) {
        throw ValidationException("Validation error");
    }
}

}
