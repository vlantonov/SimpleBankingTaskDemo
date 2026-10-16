add_library(adapter_http
    src/auth/login_controller.cpp
    src/auth/logout_controller.cpp
    src/balance/balance_controller.cpp
    src/deposit/deposit_controller.cpp
    src/exception_handler.cpp
)
target_include_directories(adapter_http PUBLIC include)
target_link_libraries(adapter_http PUBLIC usecase httplib::httplib nlohmann_json::nlohmann_json)

add_executable(adapter_http_tests
    tests/auth/login_controller_test.cpp
    tests/auth/logout_controller_test.cpp
    tests/balance/balance_controller_test.cpp
    tests/deposit/deposit_controller_test.cpp
)
target_include_directories(adapter_http_tests PRIVATE tests)
target_link_libraries(adapter_http_tests PRIVATE adapter_http GTest::gtest_main GTest::gmock)
