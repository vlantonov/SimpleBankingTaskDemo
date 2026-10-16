#include "adapters/http/exception_handler.h"
#include "domain/domain_exception.h"
#include <nlohmann/json.hpp>

namespace adapters::http {

void ExceptionHandler::handle(const std::function<void()>& action, httplib::Response& res) const {
    try {
        action();
    } catch (const domain::RateLimitException& e) {
        set_error_response(e.what(), 429, res);
    } catch (const domain::InvalidCredentialsException& e) {
        set_error_response(e.what(), 401, res);
    } catch (const domain::SessionConflictException& e) {
        set_error_response(e.what(), 409, res);
    } catch (const domain::ServerErrorException& e) {
        set_error_response(e.what(), 500, res);
    } catch (const domain::ValidationException& e) {
        set_error_response(e.what(), 400, res);
    } catch (const std::exception&) {
        set_error_response("Bad request", 400, res);
    }
}

void ExceptionHandler::set_error_response(const std::string& error, int status, httplib::Response& res) const {
    res.status = status;
    res.set_content(nlohmann::json{{"error", error}, {"token", ""}, {"message", ""}}.dump(),
                    "application/json");
}

}
