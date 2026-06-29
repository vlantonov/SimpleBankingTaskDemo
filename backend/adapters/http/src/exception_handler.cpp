#include "adapters/http/exception_handler.h"
#include <nlohmann/json.hpp>

namespace adapters::http {

void ExceptionHandler::handle(const std::function<void()>& action, httplib::Response& res) const {
    try {
        action();
    } catch (const domain::ValidationException& e) {
        set_validation_error(e, res);
    } catch (const std::exception&) {
        set_bad_request_error(res);
    }
}

void ExceptionHandler::set_bad_request_error(httplib::Response& res) const {
    set_error_response("Bad request", res);
}

void ExceptionHandler::set_validation_error(const domain::ValidationException& e, httplib::Response& res) const {
    set_error_response(e.what(), res);
}

void ExceptionHandler::set_error_response(const std::string& error, httplib::Response& res) const {
    nlohmann::json body;
    body["error"] = error;
    body["token"] = "";
    body["message"] = "";
    res.status = 400;
    res.set_content(body.dump(), "application/json");
}

}
