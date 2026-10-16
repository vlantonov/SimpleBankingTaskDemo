#include "domain/transfer/transfer_request.h"

#include <utility>

namespace domain {

TransferRequest::TransferRequest(std::string token, std::string destination_username, double amount)
	: token_(std::move(token))
	, destination_username_(std::move(destination_username))
	, amount_(amount) {}

const std::string& TransferRequest::token() const {
	return token_;
}

const std::string& TransferRequest::destination_username() const {
	return destination_username_;
}

double TransferRequest::amount() const {
	return amount_;
}

}
