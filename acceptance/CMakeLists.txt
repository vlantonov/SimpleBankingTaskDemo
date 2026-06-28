add_executable(acceptance_tests
    tests/backend/auth/auth_acceptance_test.cpp
)

target_include_directories(acceptance_tests PRIVATE
    tests
)

target_link_libraries(acceptance_tests PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
    GTest::gtest_main
)

target_compile_definitions(acceptance_tests PRIVATE
    BACKEND_PORT_DEFAULT=8080
)
