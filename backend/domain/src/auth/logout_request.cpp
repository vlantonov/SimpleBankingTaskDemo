#include "domain/auth/logout_request.h"

namespace domain {

LogoutRequest::LogoutRequest(std::string token)
    : token_(std::move(token)) {
    if (token_.empty()) {
        throw ValidationException("Validation error");
    }
}

}