#pragma once

#include <string>

class StabixExcept : public std::exception {
    private:
        std::string message;

    public:
        StabixExcept(const std::string &msg);
        const char *what() const noexcept;
        const std::string &get_message() const noexcept;
};