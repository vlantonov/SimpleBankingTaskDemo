add_executable(banking_server
    main.cpp
    server_configurator.cpp
)
target_include_directories(banking_server PRIVATE .)
target_link_libraries(banking_server PRIVATE httplib::httplib)

add_executable(server_routing_tests
    tests/server_routing_test.cpp
    server_configurator.cpp
)
target_include_directories(server_routing_tests PRIVATE .)
target_link_libraries(server_routing_tests PRIVATE httplib::httplib nlohmann_json::nlohmann_json GTest::gtest_main)
