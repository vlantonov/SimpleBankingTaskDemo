add_library(domain
    src/auth/login_request.cpp
    src/auth/logout_request.cpp
)
target_include_directories(domain PUBLIC include)
