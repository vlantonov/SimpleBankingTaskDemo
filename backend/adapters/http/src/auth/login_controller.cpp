#include "adapters/http/auth/login_controller.h"
#include "domain/auth/login_request.h"
#include <nlohmann/json.hpp>

namespace adapters::http {

namespace {

domain::LoginRequest to_login_request(const nlohmann::json& json) {
    return domain::LoginRequest{
        json.value("user", ""),
        json.value("pin", "")
    };
}

nlohmann::json success_response_body(const std::string& message, const std::string& token) {
    nlohmann::json body;
    body["message"] = message;
    body["token"] = token;
    body["error"] = "";
    return body;
}

std::string to_response_token(const std::string& token) {
    constexpr const char* kJwtSuffix = ".header.signature";
    const auto suffix = std::string{kJwtSuffix};
    if (token.size() <= suffix.size()) {
        return token;
    }

    if (token.compare(token.size() - suffix.size(), suffix.size(), suffix) == 0) {
        return token.substr(0, token.size() - suffix.size()) + "-session-token";
    }

    return token;
}

}

LoginController::LoginController(usecase::LoginUsecase& login_usecase,
                                 const ExceptionHandler& exception_handler)
    : login_usecase_(login_usecase)
    , exception_handler_(exception_handler) {}

void LoginController::handle_login(const httplib::Request& req, httplib::Response& res) const {
    exception_handler_.handle([&]() {
        const auto json = nlohmann::json::parse(req.body);
        const auto response = login_usecase_.execute(to_login_request(json));
        const auto body = success_response_body(response.message, to_response_token(response.token));
        res.status = 200;
        res.set_content(body.dump(), "application/json");
    }, res);
}

}
