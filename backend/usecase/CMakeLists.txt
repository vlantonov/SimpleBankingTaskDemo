add_library(usecase
    src/auth/login_usecase.cpp
)
target_include_directories(usecase PUBLIC include)
target_link_libraries(usecase PUBLIC domain)

add_executable(usecase_tests
    tests/auth/login/login_test.cpp
)
target_include_directories(usecase_tests PRIVATE tests)
target_link_libraries(usecase_tests PRIVATE usecase GTest::gtest_main GTest::gmock)
