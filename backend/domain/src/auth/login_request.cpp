#include "domain/auth/login_request.h"

namespace domain {

LoginRequest::LoginRequest(std::string username, std::string pin)
    : username_(std::move(username))
    , pin_(std::move(pin)) {
    if (username_.empty() || pin_.empty()) {
        throw ValidationException("Validation error");
    }
}

}
