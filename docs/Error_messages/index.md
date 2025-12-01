# Error Handling in R-Type

## Overview
The `Error` class in R-Type is designed to provide a structured and consistent way to handle errors throughout the project. It categorizes errors into different types and associates them with descriptive messages to help developers debug and resolve issues efficiently.

## Error Types
The `Error` class supports the following error types:

- **NetworkError**: For issues related to network communication.
- **GameplayError**: For errors occurring during gameplay logic.
- **ResourceError**: For problems with loading or accessing resources.
- **ConfigurationError**: For invalid or missing configuration parameters.
- **SystemError**: For system-level issues such as permissions or hardware limitations.

## Usage

### Throwing an Error
To throw an error, create an instance of the `Error` class with the appropriate `ErrorType` and message. For example:

```cpp
#include "Error.hpp"
#include "ErrorMessages.hpp"

void connectToServer() {
    throw Error(ErrorType::NetworkError, ErrorMessages::NETWORK_ERROR);
}
```

### Catching an Error
Use a `try-catch` block to handle errors. You can retrieve the error message and type as follows:

```cpp
try {
    connectToServer();
} catch (const Error &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    if (e.getType() == ErrorType::NetworkError) {
        std::cerr << "Please check your network connection." << std::endl;
    }
}
```

## Best Practices
- Always use the predefined messages in `ErrorMessages.hpp` to ensure consistency.
- Categorize errors appropriately using `ErrorType`.
- Provide meaningful and actionable messages to help with debugging.

By following these guidelines, you can ensure that error handling in R-Type is robust and easy to maintain.