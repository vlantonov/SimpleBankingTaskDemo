#pragma once
#include <httplib.h>
#include <functional>
#include <string>

namespace adapters::http {

class ExceptionHandler {
public:
    void handle(const std::function<void()>& action, httplib::Response& res) const;

private:
    void set_error_response(const std::string& error, int status, httplib::Response& res) const;
};

}
