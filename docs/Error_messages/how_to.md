# How to Add New Error Messages

## Overview
This guide explains how to add new error messages to the `Error` class in R-Type. By following these steps, you can ensure that all error messages are consistent and easy to maintain.

## Steps to Add a New Error Message

### 1. Define a New Error Type (if needed)
If the new error does not fit into an existing `ErrorType`, add a new type to the `ErrorType` enum in `Error.hpp`.

```cpp
// Error.hpp
enum class ErrorType {
    NetworkError,
    GameplayError,
    ResourceError,
    ConfigurationError,
    SystemError,
    NewErrorType // Add your new error type here
};
```

### 2. Add the Error Message
Define the new error message in `ErrorMessages.hpp`. Use a descriptive and concise message.

```cpp
// ErrorMessages.hpp
namespace ErrorMessages {
    constexpr const char *NEW_ERROR_MESSAGE = "Description of the new error.";
}
```

### 3. Use the New Error
You can now use the new error type and message in your code. For example:

```cpp
#include "Error.hpp"
#include "ErrorMessages.hpp"

void exampleFunction() {
    throw Error(ErrorType::NewErrorType, ErrorMessages::NEW_ERROR_MESSAGE);
}
```

### 4. Test the New Error
Ensure that the new error is properly handled in your code. Use a `try-catch` block to verify its behavior.

```cpp
try {
    exampleFunction();
} catch (const Error &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    if (e.getType() == ErrorType::NewErrorType) {
        std::cerr << "Handle the new error here." << std::endl;
    }
}
```

## Best Practices
- Keep error messages clear and concise.
- Use the `ErrorType` enum to categorize errors logically.
- Always test new error messages to ensure they integrate seamlessly with the existing system.

By following these steps, you can easily extend the error handling system in R-Type to accommodate new scenarios.