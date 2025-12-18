/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Error
*/

#include <common/error/Error.hpp>

Error::Error(ErrorType type, const std::string &message) : _type(type), _message(message) {}

const char *Error::what() const noexcept {
    return _message.c_str();
}

ErrorType Error::getType() const noexcept {
    return _type;
}
