#pragma once
#include <stdexcept>
#include <string>

namespace domain {

class DomainException : public std::runtime_error {
public:
    explicit DomainException(const std::string& message)
        : std::runtime_error(message) {}
};

class ValidationException : public DomainException {
public:
    explicit ValidationException(const std::string& message)
        : DomainException(message) {}
};

}
