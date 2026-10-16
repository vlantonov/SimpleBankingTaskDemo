add_library(domain
    src/auth/login_request.cpp
    src/auth/logout_request.cpp
    src/balance/balance_request.cpp
    src/deposit/deposit_request.cpp
    src/history/history_request.cpp
    src/transfer/transfer_request.cpp
    src/withdraw/withdraw_request.cpp
)
target_include_directories(domain PUBLIC include)
