#include "stabix_except.h"

StabixExcept::StabixExcept(const std::string &msg) : message(msg) {}

const char* StabixExcept::what() const noexcept {
    return this->message.c_str();
}

const std::string& StabixExcept::get_message() const noexcept {
    return this->message;
}