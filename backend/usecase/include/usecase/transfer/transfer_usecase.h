#pragma once
#include <memory>
#include <optional>
#include <string>
#include "domain/transfer/transfer_request.h"

namespace usecase {

class ITransferSessionPort {
public:
    virtual ~ITransferSessionPort() = default;
    virtual std::optional<std::string> find_username_by_token(const std::string& token) const = 0;
};

class ITransferAccountPort {
public:
    virtual ~ITransferAccountPort() = default;
    virtual std::optional<double> find_balance_for_username(const std::string& username) const = 0;
    virtual bool destination_exists(const std::string& username) const = 0;
    virtual void append_transfer_between(const std::string& source_username,
                                         const std::string& destination_username,
                                         double amount) = 0;
};

struct TransferResponse {
    double source_balance{};
    double destination_balance{};

    bool operator==(const TransferResponse&) const = default;
};

class TransferUsecase {
public:
    TransferUsecase(std::unique_ptr<ITransferSessionPort> session_port,
                    std::unique_ptr<ITransferAccountPort> account_port);

    TransferResponse execute(const domain::TransferRequest& request) const;

private:
    std::string require_source_username(const domain::TransferRequest& request) const;
    std::string require_destination_username(const domain::TransferRequest& request) const;
    double find_starting_balance(const std::string& username) const;
    void append_transfer(const std::string& source_username,
                         const std::string& destination_username,
                         double amount) const;
    TransferResponse build_transfer_response(double source_starting_balance,
                                            double destination_starting_balance,
                                            double amount) const;

    std::unique_ptr<ITransferSessionPort> session_port_;
    std::unique_ptr<ITransferAccountPort> account_port_;
};

}
