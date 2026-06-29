#pragma once
#include <httplib.h>
#include <functional>
#include "domain/domain_exception.h"

namespace adapters::http {

class ExceptionHandler {
public:
    void handle(const std::function<void()>& action, httplib::Response& res) const;

private:
    void set_validation_error(const domain::ValidationException& e, httplib::Response& res) const;
    void set_bad_request_error(httplib::Response& res) const;
    void set_error_response(const std::string& error, httplib::Response& res) const;
};

}
