#pragma once
#include <httplib.h>
#include <functional>

namespace adapters::http {

class ExceptionHandler {
public:
    void handle(const std::function<void()>& action, httplib::Response& res) const;
};

}
