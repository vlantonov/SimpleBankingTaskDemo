#pragma once
#include "domain/auth/login_request.h"
#include "usecase/auth/login_response.h"

namespace usecase {

class LoginUsecase {
public:
    LoginResponse execute(const domain::LoginRequest& request);
};

}
