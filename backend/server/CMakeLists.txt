add_executable(banking_server main.cpp)
target_link_libraries(banking_server PRIVATE httplib::httplib)
