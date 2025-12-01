#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>
#include <string>

// Enum for error types
enum class ErrorType {
    NetworkError,
    GameplayError,
    ResourceError,
    ConfigurationError,
    SystemError
};

class Error : public std::exception {
public:
    Error(ErrorType type, const std::string &message);

    const char *what() const noexcept override;
    ErrorType getType() const noexcept;

private:
    ErrorType _type;
    std::string _message;
};

#endif // ERROR_HPP
