#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace dto::deposit {

struct DepositRequest {
    double amount{};

    std::string to_json_string() const {
        return nlohmann::json{{"amount", amount}}.dump();
    }
};

}
