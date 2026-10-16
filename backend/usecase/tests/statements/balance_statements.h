#pragma once
#include <stdexcept>

class BalanceStatements {
public:
    void assert_balance_request_without_a_valid_session_is_rejected() {
        throw std::logic_error("Not implemented");
    }

    void assert_balance_request_with_a_fabricated_token_is_rejected() {
        throw std::logic_error("Not implemented");
    }

    void assert_authenticated_user_receives_the_current_balance() {
        throw std::logic_error("Not implemented");
    }
};
