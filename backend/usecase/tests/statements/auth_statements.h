#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "usecase/auth/login_usecase.h"
#include "domain/domain_exception.h"
#include "scope/login_scope.h"

class AuthStatements {
public:
    explicit AuthStatements(usecase::LoginUsecase& login_usecase)
        : login_usecase_(login_usecase) {}

    void assert_login_rejected_with_validation_error() {
        auto scope = LoginScope::Builder().with_username("").with_pin("123").build();
        EXPECT_THAT(
            [&]() { login_usecase_.execute(scope.to_login_request()); },
            testing::ThrowsMessage<domain::ValidationException>(testing::Eq("Validation error"))
        );
    }

private:
    usecase::LoginUsecase& login_usecase_;
};
