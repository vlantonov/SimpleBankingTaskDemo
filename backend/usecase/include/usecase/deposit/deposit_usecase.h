#pragma once
#include <memory>
#include <optional>
#include <string>
#include "domain/deposit/deposit_request.h"

namespace usecase {

class IDepositSessionPort {
public:
    virtual ~IDepositSessionPort() = default;
    virtual std::optional<std::string> find_username_by_token(const std::string& token) const = 0;
};

class IAccountingPort {
public:
    virtual ~IAccountingPort() = default;
    virtual std::optional<double> find_balance_for_username(const std::string& username) const = 0;
    virtual void append_deposit_for_username(const std::string& username, double amount) = 0;
};

struct DepositResponse {
    double balance{};
};

class DepositUsecase {
public:
    DepositUsecase(std::unique_ptr<IDepositSessionPort> session_port,
                   std::unique_ptr<IAccountingPort> account_port);

    DepositResponse execute(const domain::DepositRequest& request) const;

private:
    std::unique_ptr<IDepositSessionPort> session_port_;
    std::unique_ptr<IAccountingPort> account_port_;
};

}
