#pragma once
#include <memory>
#include <optional>
#include <string>
#include "domain/withdraw/withdraw_request.h"

namespace usecase {

class IWithdrawSessionPort {
public:
    virtual ~IWithdrawSessionPort() = default;
    virtual std::optional<std::string> find_username_by_token(const std::string& token) const = 0;
};

class IAccountingPort {
public:
    virtual ~IAccountingPort() = default;
    virtual std::optional<double> find_balance_for_username(const std::string& username) const = 0;
    virtual void append_withdraw_for_username(const std::string& username, double amount) = 0;
};

struct WithdrawResponse {
    double balance{};
};

class WithdrawUsecase {
public:
    WithdrawUsecase(std::unique_ptr<IWithdrawSessionPort> session_port,
                    std::unique_ptr<IAccountingPort> account_port);

    WithdrawResponse execute(const domain::WithdrawRequest& request) const;

private:
    std::unique_ptr<IWithdrawSessionPort> session_port_;
    std::unique_ptr<IAccountingPort> account_port_;
};

}
