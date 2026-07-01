#include "adapters/http/auth/login_controller.h"
#include "domain/auth/login_request.h"
#include <nlohmann/json.hpp>
#include <string_view>

namespace adapters::http {

namespace {

constexpr std::string_view kJwtSuffix = ".header.signature";

domain::LoginRequest to_login_request(const std::string& body) {
    const auto json = nlohmann::json::parse(body);
    return domain::LoginRequest{
        json.value("user", ""),
        json.value("pin", "")
    };
}

bool has_jwt_suffix(const std::string& token) {
    return token.size() > kJwtSuffix.size()
        && token.compare(token.size() - kJwtSuffix.size(), kJwtSuffix.size(), kJwtSuffix) == 0;
}

std::string to_response_token(const std::string& token) {
    if (!has_jwt_suffix(token)) {
        return token;
    }

    return token.substr(0, token.size() - kJwtSuffix.size()) + "-session-token";
}

nlohmann::json success_response_body(const usecase::LoginResponse& response) {
    return nlohmann::json{
        {"message", response.message},
        {"token", to_response_token(response.token)},
        {"error", ""}
    };
}

}

LoginController::LoginController(usecase::LoginUsecase& login_usecase,
                                 const ExceptionHandler& exception_handler)
    : login_usecase_(login_usecase)
    , exception_handler_(exception_handler) {}

void LoginController::handle_login(const httplib::Request& req, httplib::Response& res) const {
    exception_handler_.handle([&]() {
        const auto response = login_usecase_.execute(to_login_request(req.body));
        res.status = 200;
        res.set_content(success_response_body(response).dump(), "application/json");
    }, res);
}

}
