#include <gtest/gtest.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <unordered_map>
#include "adapters/http/transfer/transfer_controller.h"
#include "adapters/http/exception_handler.h"
#include "usecase/transfer/transfer_usecase.h"

namespace {

class FakeTransferSessionPort : public usecase::ITransferSessionPort {
public:
    void given_active_session(const std::string& token, const std::string& username) {
        sessions_[token] = username;
    }

    std::optional<std::string> find_username_by_token(const std::string& token) const override {
        const auto it = sessions_.find(token);
        if (it == sessions_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

private:
    std::unordered_map<std::string, std::string> sessions_;
};

class FakeTransferAccountPort : public usecase::ITransferAccountPort {
public:
    std::optional<double> find_balance_for_username(const std::string& username) const override {
        if (username == "ola") return ola_balance_;
        if (username == "marta") return marta_balance_;
        return std::nullopt;
    }

    bool destination_exists(const std::string& username) const override {
        return username == "marta";
    }

    void append_transfer_between(const std::string& source_username,
                                 const std::string& destination_username,
                                 double amount) override {
        if (source_username == "ola") ola_balance_ -= amount;
        if (destination_username == "marta") marta_balance_ += amount;
    }

    double ola_balance_{200.0};
    double marta_balance_{50.0};
};

}

class TransferControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto session_port = std::make_unique<FakeTransferSessionPort>();
        fake_session_port_ = session_port.get();

        usecase_ = std::make_unique<usecase::TransferUsecase>(
            std::move(session_port),
            std::make_unique<FakeTransferAccountPort>());
        controller_ = std::make_unique<adapters::http::TransferController>(*usecase_, exception_handler_);
    }

    FakeTransferSessionPort* fake_session_port_ = nullptr;
    std::unique_ptr<usecase::TransferUsecase> usecase_;
    adapters::http::ExceptionHandler exception_handler_;
    std::unique_ptr<adapters::http::TransferController> controller_;
};

TEST_F(TransferControllerTest, DISABLED_transfer_without_session_token_is_rejected_with_authentication_error) {
    SCOPED_TRACE("Scenario 1.1: HTTP adapter — transfer without token returns 401 Authentication error");

    httplib::Request req;
    req.body = R"({"destinationUsername":"marta","amount":40.0})";
    httplib::Response res;

    controller_->handle_transfer(req, res);

    const auto body = nlohmann::json::parse(res.body);
    EXPECT_EQ(res.status, 401);
    EXPECT_EQ(body.value("error", ""), "Authentication error");
    EXPECT_EQ(body.value("message", "missing"), "");
}

TEST_F(TransferControllerTest, DISABLED_authenticated_user_receives_updated_source_and_destination_balances) {
    SCOPED_TRACE("Scenario 3.1: HTTP adapter — valid transfer request returns updated source and destination balances");

    fake_session_port_->given_active_session("ola.header.signature", "ola");

    httplib::Request req;
    req.set_header("Authorization", "Bearer ola.header.signature");
    req.body = R"({"destinationUsername":"marta","amount":40.0})";
    httplib::Response res;

    controller_->handle_transfer(req, res);

    const auto body = nlohmann::json::parse(res.body);
    EXPECT_EQ(res.status, 201);
    EXPECT_EQ(body.value("sourceBalance", 0.0), 160.0);
    EXPECT_EQ(body.value("destinationBalance", 0.0), 90.0);
    EXPECT_EQ(body.value("error", "missing"), "");
    EXPECT_EQ(body.value("message", ""), "Transfer successful");
}
