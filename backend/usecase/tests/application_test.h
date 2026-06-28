#pragma once
#include <gtest/gtest.h>
#include "usecase/auth/login_usecase.h"
#include "statements/auth_statements.h"

class ApplicationTest : public ::testing::Test {
protected:
    usecase::LoginUsecase login_usecase_;
    AuthStatements auth_statements_{login_usecase_};
};
