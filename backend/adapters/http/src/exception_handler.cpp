#include "adapters/http/exception_handler.h"
#include <stdexcept>

namespace adapters::http {

void ExceptionHandler::handle(const std::function<void()>& action, httplib::Response& res) const {
    throw std::logic_error("Not implemented");
}

}
