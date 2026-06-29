#include "adapters/http/exception_handler.h"
#include "domain/domain_exception.h"
#include <nlohmann/json.hpp>

namespace adapters::http {

void ExceptionHandler::handle(const std::function<void()>& action, httplib::Response& res) const {
    try {
        action();
    } catch (const domain::ValidationException& e) {
        nlohmann::json body;
        body["error"] = e.what();
        body["token"] = "";
        body["message"] = "";
        res.status = 400;
        res.set_content(body.dump(), "application/json");
    }
}

}
