#include "domain/auth/login_request.h"

namespace domain {

LoginRequest::LoginRequest(std::string username, std::string pin)
    : username(std::move(username))
    , pin(std::move(pin)) {
    throw std::logic_error("Not implemented");
}

}
