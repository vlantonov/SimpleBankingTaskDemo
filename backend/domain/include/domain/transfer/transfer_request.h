#pragma once
#include <string>

namespace domain {

struct TransferRequest {
    TransferRequest(std::string token, std::string destination_username, double amount);
};

}
