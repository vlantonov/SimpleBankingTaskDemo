#pragma once
#include <httplib.h>
#include <string>
#include "dto/auth/login_request.h"
#include "dto/auth/login_response.h"
#include "dto/auth/logout_response.h"
#include "dto/balance/balance_response.h"
#include "dto/deposit/deposit_request.h"
#include "dto/deposit/deposit_response.h"
#include "dto/history/history_response.h"
#include "dto/withdraw/withdraw_request.h"
#include "dto/withdraw/withdraw_response.h"

class ApplicationClient {
public:
    explicit ApplicationClient(const std::string& host, int port)
        : client_(host, port) {}

    dto::auth::LoginResponse login(const dto::auth::LoginRequest& req) {
        auto res = client_.Post("/api/v1/login", req.to_json_string(), "application/json");
        if (!res) return dto::auth::LoginResponse::connection_failed();
        return dto::auth::LoginResponse::parse(res->status, res->body);
    }

    dto::auth::LogoutResponse logout(const std::string& token) {
        httplib::Headers headers{{"Authorization", "Bearer " + token}};
        auto res = client_.Post("/api/v1/logout", headers, "", "application/json");
        if (!res) return dto::auth::LogoutResponse::connection_failed();
        return dto::auth::LogoutResponse::parse(res->status, res->body);
    }

    dto::auth::LogoutResponse logout_without_token() {
        auto res = client_.Post("/api/v1/logout", "", "application/json");
        if (!res) return dto::auth::LogoutResponse::connection_failed();
        return dto::auth::LogoutResponse::parse(res->status, res->body);
    }

    dto::balance::BalanceResponse balance(const std::string& token) {
        httplib::Headers headers{{"Authorization", "Bearer " + token}};
        auto res = client_.Get("/api/v1/balance", headers);
        if (!res) return dto::balance::BalanceResponse::connection_failed();
        return dto::balance::BalanceResponse::parse(res->status, res->body);
    }

    dto::balance::BalanceResponse balance_without_token() {
        auto res = client_.Get("/api/v1/balance");
        if (!res) return dto::balance::BalanceResponse::connection_failed();
        return dto::balance::BalanceResponse::parse(res->status, res->body);
    }

    dto::deposit::DepositResponse deposit(const std::string& token, double amount) {
        httplib::Headers headers{{"Authorization", "Bearer " + token}};
        const auto request = dto::deposit::DepositRequest{amount};
        auto res = client_.Post("/api/v1/deposit", headers, request.to_json_string(), "application/json");
        if (!res) return dto::deposit::DepositResponse::connection_failed();
        return dto::deposit::DepositResponse::parse(res->status, res->body);
    }

    dto::deposit::DepositResponse deposit_without_token(double amount) {
        const auto request = dto::deposit::DepositRequest{amount};
        auto res = client_.Post("/api/v1/deposit", request.to_json_string(), "application/json");
        if (!res) return dto::deposit::DepositResponse::connection_failed();
        return dto::deposit::DepositResponse::parse(res->status, res->body);
    }

    dto::withdraw::WithdrawResponse withdraw(const std::string& token, double amount) {
        httplib::Headers headers{{"Authorization", "Bearer " + token}};
        const auto request = dto::withdraw::WithdrawRequest{amount};
        auto res = client_.Post("/api/v1/withdraw", headers, request.to_json_string(), "application/json");
        if (!res) return dto::withdraw::WithdrawResponse::connection_failed();
        return dto::withdraw::WithdrawResponse::parse(res->status, res->body);
    }

    dto::withdraw::WithdrawResponse withdraw_without_token(double amount) {
        const auto request = dto::withdraw::WithdrawRequest{amount};
        auto res = client_.Post("/api/v1/withdraw", request.to_json_string(), "application/json");
        if (!res) return dto::withdraw::WithdrawResponse::connection_failed();
        return dto::withdraw::WithdrawResponse::parse(res->status, res->body);
    }

    dto::history::HistoryResponse history(const std::string& token) {
        httplib::Headers headers{{"Authorization", "Bearer " + token}};
        auto res = client_.Get("/api/v1/history", headers);
        if (!res) return dto::history::HistoryResponse::connection_failed();
        return dto::history::HistoryResponse::parse(res->status, res->body);
    }

    dto::history::HistoryResponse history_without_token() {
        auto res = client_.Get("/api/v1/history");
        if (!res) return dto::history::HistoryResponse::connection_failed();
        return dto::history::HistoryResponse::parse(res->status, res->body);
    }

private:
    httplib::Client client_;
};
