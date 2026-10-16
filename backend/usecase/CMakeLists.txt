add_library(usecase
    src/auth/login_usecase.cpp
    src/auth/logout_usecase.cpp
    src/balance/get_balance_usecase.cpp
    src/deposit/deposit_usecase.cpp
)
target_include_directories(usecase PUBLIC include)
target_link_libraries(usecase PUBLIC domain)

add_executable(usecase_tests
    tests/auth/login/login_test.cpp
    tests/auth/logout/logout_test.cpp
    tests/balance/get_balance_test.cpp
    tests/deposit/deposit_test.cpp
)
target_include_directories(usecase_tests PRIVATE tests)
target_link_libraries(usecase_tests PRIVATE usecase GTest::gtest_main GTest::gmock)
