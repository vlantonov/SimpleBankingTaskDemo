#pragma once
#include <string>

namespace domain {

struct TransferRequest {
    TransferRequest(std::string token, std::string destination_username, double amount);

    const std::string& token() const;
    const std::string& destination_username() const;
    double amount() const;

private:
    std::string token_;
    std::string destination_username_;
    double amount_{};
};

}
