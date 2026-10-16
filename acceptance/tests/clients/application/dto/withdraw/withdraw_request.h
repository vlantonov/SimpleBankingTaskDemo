#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace dto::withdraw {

struct WithdrawRequest {
    double amount{};

    std::string to_json_string() const {
        return nlohmann::json{{"amount", amount}}.dump();
    }
};

}
