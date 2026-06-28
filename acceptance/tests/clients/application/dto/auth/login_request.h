#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace dto::auth {

struct LoginRequest {
    std::string user;
    std::string pin;

    std::string to_json_string() const {
        return nlohmann::json{{"user", user}, {"pin", pin}}.dump();
    }
};

}
