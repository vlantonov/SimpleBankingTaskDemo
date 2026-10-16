#pragma once
#include <string>

namespace domain {

struct HistoryRequest {
    const std::string token_;

    explicit HistoryRequest(std::string token);
};

}
