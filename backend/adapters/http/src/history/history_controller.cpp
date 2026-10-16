#include "adapters/http/history/history_controller.h"
#include "domain/domain_exception.h"
#include "domain/history/history_request.h"
#include <nlohmann/json.hpp>
#include <string_view>

namespace adapters::http {

namespace {

constexpr std::string_view kBearerPrefix = "Bearer ";
constexpr std::string_view kExternalTokenSuffix = "-session-token";
constexpr std::string_view kInternalTokenSuffix = ".header.signature";

bool has_internal_token_suffix(const std::string& token) {
    return token.size() > kInternalTokenSuffix.size()
        && token.compare(token.size() - kInternalTokenSuffix.size(),
                         kInternalTokenSuffix.size(),
                         kInternalTokenSuffix)
                == 0;
}

bool has_external_token_suffix(const std::string& token) {
    return token.size() > kExternalTokenSuffix.size()
        && token.compare(token.size() - kExternalTokenSuffix.size(),
                         kExternalTokenSuffix.size(),
                         kExternalTokenSuffix)
                == 0;
}

bool has_bearer_prefix(std::string_view auth_header) {
    return auth_header.size() > kBearerPrefix.size()
        && auth_header.compare(0, kBearerPrefix.size(), kBearerPrefix) == 0;
}

std::string to_internal_token(const std::string& token) {
    if (has_internal_token_suffix(token)) {
        return token;
    }
    if (!has_external_token_suffix(token)) {
        return "";
    }

    return token.substr(0, token.size() - kExternalTokenSuffix.size())
        + std::string{kInternalTokenSuffix};
}

std::string extract_authorization_token(const httplib::Request& req) {
    const auto auth_header = req.get_header_value("Authorization");
    if (!has_bearer_prefix(auth_header)) {
        return "";
    }

    return to_internal_token(auth_header.substr(kBearerPrefix.size()));
}

std::string extract_authorization_token_or_throw(const httplib::Request& req) {
    const auto token = extract_authorization_token(req);
    if (token.empty()) {
        throw domain::InvalidCredentialsException{"Authentication error"};
    }
    return token;
}

nlohmann::json success_response_body(const std::vector<usecase::HistoryEntry>& entries) {
    auto transactions = nlohmann::json::array();
    for (const auto& entry : entries) {
        transactions.push_back({
            {"type", entry.type},
            {"amount", entry.amount},
            {"timestamp", entry.timestamp}
        });
    }
    return nlohmann::json{{"transactions", transactions}};
}

}

HistoryController::HistoryController(usecase::GetHistoryUsecase& get_history_usecase,
                                     const ExceptionHandler& exception_handler)
    : get_history_usecase_(get_history_usecase)
    , exception_handler_(exception_handler) {}

void HistoryController::handle_history(const httplib::Request& req, httplib::Response& res) const {
    exception_handler_.handle([&]() {
        const auto request = domain::HistoryRequest{extract_authorization_token_or_throw(req)};
        const auto response = get_history_usecase_.execute(request);
        res.status = 200;
        res.set_content(success_response_body(response.entries).dump(), "application/json");
    }, res);
}

}
