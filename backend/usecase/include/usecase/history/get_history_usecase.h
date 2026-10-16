#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "domain/history/history_request.h"

namespace usecase {

class IGetHistorySessionPort {
public:
    virtual ~IGetHistorySessionPort() = default;
    virtual std::optional<std::string> find_username_by_token(const std::string& token) const = 0;
};

struct HistoryEntry {
    std::string kind{};
    double amount{};
    std::string description{};
};

class IHistoryAccountPort {
public:
    virtual ~IHistoryAccountPort() = default;
    virtual std::vector<HistoryEntry> find_history_for_username(const std::string& username) const = 0;
};

struct HistoryResponse {
    std::vector<HistoryEntry> entries{};
};

class GetHistoryUsecase {
public:
    GetHistoryUsecase(std::unique_ptr<IGetHistorySessionPort> session_port,
                      std::unique_ptr<IHistoryAccountPort> account_port);

    HistoryResponse execute(const domain::HistoryRequest& request) const;

private:
    std::unique_ptr<IGetHistorySessionPort> session_port_;
    std::unique_ptr<IHistoryAccountPort> account_port_;
};

}
