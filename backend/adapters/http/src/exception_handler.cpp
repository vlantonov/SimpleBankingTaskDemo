#include "adapters/http/exception_handler.h"
#include <nlohmann/json.hpp>

namespace adapters::http {

void ExceptionHandler::handle(const std::function<void()>& action, httplib::Response& res) const {
    try {
        action();
    } catch (const domain::InvalidCredentialsException& e) {
        set_invalid_credentials_error(e, res);
    } catch (const domain::ValidationException& e) {
        set_validation_error(e, res);
    } catch (const std::exception&) {
        set_bad_request_error(res);
    }
}

void ExceptionHandler::set_invalid_credentials_error(const domain::InvalidCredentialsException& e,
                                                    httplib::Response& res) const {
    set_error_response(e.what(), res);
    res.status = 401;
}

void ExceptionHandler::set_bad_request_error(httplib::Response& res) const {
    set_error_response("Bad request", res);
}

void ExceptionHandler::set_validation_error(const domain::ValidationException& e, httplib::Response& res) const {
    set_error_response(e.what(), res);
}

void ExceptionHandler::set_error_response(const std::string& error, httplib::Response& res) const {
    res.status = 400;
    res.set_content(nlohmann::json{{"error", error}, {"token", ""}, {"message", ""}}.dump(),
                    "application/json");
}

}
