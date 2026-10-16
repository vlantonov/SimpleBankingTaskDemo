#pragma once
#include <memory>
#include <optional>
#include <string>
#include "domain/balance/balance_request.h"

namespace usecase {

class IGetBalanceSessionPort {
public:
    virtual ~IGetBalanceSessionPort() = default;
    virtual std::optional<std::string> find_username_by_token(const std::string& token) const = 0;
};

class IAccountPort {
public:
    virtual ~IAccountPort() = default;
    virtual std::optional<double> find_balance_for_username(const std::string& username) const = 0;
};

struct BalanceResponse {
    double balance{};
};

class GetBalanceUsecase {
public:
    GetBalanceUsecase(std::unique_ptr<IGetBalanceSessionPort> session_port,
                      std::unique_ptr<IAccountPort> account_port);

    BalanceResponse execute(const domain::BalanceRequest& request) const;

private:
    std::unique_ptr<IGetBalanceSessionPort> session_port_;
    std::unique_ptr<IAccountPort> account_port_;
};

using BalanceUsecase = GetBalanceUsecase;

}
