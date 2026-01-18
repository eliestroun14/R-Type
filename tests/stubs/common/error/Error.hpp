/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Stub Error for Game.cpp coverage tests
*/

#ifndef TESTS_STUB_ERROR_HPP_
#define TESTS_STUB_ERROR_HPP_

#include <stdexcept>
#include <string>

enum class ErrorType {
    GameplayError,
    ServerError
};

class Error : public std::runtime_error {
public:
    Error(ErrorType type, const std::string& message)
        : std::runtime_error(message), _type(type) {}

    ErrorType type() const noexcept { return _type; }

private:
    ErrorType _type;
};

#endif /* TESTS_STUB_ERROR_HPP_ */
