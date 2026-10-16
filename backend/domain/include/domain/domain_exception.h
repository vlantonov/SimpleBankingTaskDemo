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

class InvalidCredentialsException : public DomainException {
public:
    explicit InvalidCredentialsException(const std::string& message)
        : DomainException(message) {}
};

class RateLimitException : public DomainException {
public:
    explicit RateLimitException(const std::string& message)
        : DomainException(message) {}
};

class SessionConflictException : public DomainException {
public:
    explicit SessionConflictException(const std::string& message)
        : DomainException(message) {}
};

class ServerErrorException : public DomainException {
public:
    explicit ServerErrorException(const std::string& message)
        : DomainException(message) {}
};

}
