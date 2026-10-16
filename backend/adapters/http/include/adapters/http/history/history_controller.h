#pragma once
#include <httplib.h>
#include "adapters/http/exception_handler.h"
#include "usecase/history/get_history_usecase.h"

namespace adapters::http {

class HistoryController {
public:
    explicit HistoryController(usecase::GetHistoryUsecase& get_history_usecase,
                               const ExceptionHandler& exception_handler);

    void handle_history(const httplib::Request& req, httplib::Response& res) const;

private:
    usecase::GetHistoryUsecase& get_history_usecase_;
    const ExceptionHandler& exception_handler_;
};

}
