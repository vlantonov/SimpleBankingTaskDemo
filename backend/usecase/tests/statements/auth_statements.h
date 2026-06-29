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
        EXPECT_THAT(
            [&]() { login_usecase_.execute(login_scope_with_empty_username().to_login_request()); },
            testing::ThrowsMessage<domain::ValidationException>(testing::Eq("Validation error"))
        );
    }

    void assert_first_login_creates_user_and_opens_session() {
        EXPECT_THAT(
            login_usecase_.execute(login_scope_for_first_login().to_login_request()),
            testing::AllOf(
                testing::Field(&usecase::LoginResponse::message, testing::Eq("Welcome, ola")),
                testing::Field(&usecase::LoginResponse::token, testing::Not(testing::IsEmpty()))
            )
        );
    }

private:
    LoginScope login_scope_with_empty_username() const {
        return LoginScope::Builder().with_username("").with_pin("123").build();
    }

    LoginScope login_scope_for_first_login() const {
        return LoginScope::Builder().with_username("ola").with_pin("123").build();
    }

    usecase::LoginUsecase& login_usecase_;
};
