#include "domain/history/history_request.h"

namespace domain {

HistoryRequest::HistoryRequest(std::string token)
    : token_(std::move(token)) {}

}
