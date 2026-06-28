#pragma once
#include "domain/auth/login_request.h"

namespace usecase {

class LoginUsecase {
public:
    void execute(const domain::LoginRequest& request);
};

}
