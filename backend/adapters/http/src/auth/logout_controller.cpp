#include "adapters/http/auth/logout_controller.h"
#include "domain/domain_exception.h"
#include "domain/auth/logout_request.h"
#include <nlohmann/json.hpp>
#include <string_view>

namespace adapters::http {

namespace {

constexpr std::string_view kBearerPrefix = "Bearer ";
constexpr std::string_view kExternalTokenSuffix = "-session-token";
constexpr std::string_view kInternalTokenSuffix = ".header.signature";

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
    if (!has_external_token_suffix(token)) {
        return "";
    }

    return token.substr(0, token.size() - kExternalTokenSuffix.size()) + std::string{kInternalTokenSuffix};
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

nlohmann::json success_response_body() {
    return nlohmann::json{{"error", ""}, {"message", ""}, {"token", ""}};
}

}

LogoutController::LogoutController(usecase::LogoutUsecase& logout_usecase,
                                   const ExceptionHandler& exception_handler)
    : logout_usecase_(logout_usecase)
    , exception_handler_(exception_handler) {}

void LogoutController::handle_logout(const httplib::Request& req, httplib::Response& res) const {
    exception_handler_.handle([&]() {
        const auto request = domain::LogoutRequest{extract_authorization_token_or_throw(req)};
        logout_usecase_.execute(request);
        res.status = 200;
        res.set_content(success_response_body().dump(), "application/json");
    }, res);
}

}
