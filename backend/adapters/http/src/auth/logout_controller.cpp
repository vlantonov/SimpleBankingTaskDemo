#include "adapters/http/auth/logout_controller.h"
#include "domain/auth/logout_request.h"
#include <nlohmann/json.hpp>
#include <string_view>

namespace adapters::http {

namespace {

constexpr std::string_view kBearerPrefix = "Bearer ";
constexpr std::string_view kExternalTokenSuffix = "-session-token";
constexpr std::string_view kInternalTokenSuffix = ".header.signature";

std::string to_internal_token(const std::string& token) {
    if (token.size() <= kExternalTokenSuffix.size()) {
        return token;
    }

    if (token.compare(token.size() - kExternalTokenSuffix.size(),
                      kExternalTokenSuffix.size(),
                      kExternalTokenSuffix)
        != 0) {
        return token;
    }

    return token.substr(0, token.size() - kExternalTokenSuffix.size()) + std::string{kInternalTokenSuffix};
}

std::string extract_authorization_token(const httplib::Request& req) {
    const auto auth_header = req.get_header_value("Authorization");
    if (auth_header.size() <= kBearerPrefix.size()) {
        return "";
    }

    if (auth_header.compare(0, kBearerPrefix.size(), kBearerPrefix) != 0) {
        return "";
    }

    return to_internal_token(auth_header.substr(kBearerPrefix.size()));
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
        logout_usecase_.execute(domain::LogoutRequest{extract_authorization_token(req)});
        res.status = 200;
        res.set_content(success_response_body().dump(), "application/json");
    }, res);
}

}
