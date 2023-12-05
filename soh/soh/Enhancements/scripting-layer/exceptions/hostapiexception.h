#pragma once

#include <exception>
#include <string>

class HostAPIException : public std::exception {
    std::string mMessage;
public:
    explicit HostAPIException(std::string message) : mMessage(std::move(message)) {}
    [[nodiscard]] const char* what() const noexcept override {
        return mMessage.c_str();
    }
};