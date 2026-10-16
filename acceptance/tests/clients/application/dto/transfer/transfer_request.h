#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace dto::transfer {

struct TransferRequest {
    std::string destination_user;
    double amount{};

    std::string to_json_string() const {
        return nlohmann::json{{"destinationUser", destination_user}, {"amount", amount}}.dump();
    }
};

}
