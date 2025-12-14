# Logger Documentation

## Overview

The Logger system is built on top of [spdlog](https://github.com/gabime/spdlog). It provides:

- **Multiple log levels**: Trace, Debug, Info, Warning, Error, Critical
- **Category filtering**: Exclude specific categories from logging
- **Multiple outputs**: Console (colored) and file logging
- **Automatic source location tracking**: File name and line number
- **Thread-safe operations**: Safe for multithreaded applications
- **Singleton pattern**: Global access to the logger instance

## Key Features

### 1. **Log Levels**

Different severity levels for different types of messages:

- **Trace**: Most verbose, very detailed information
- **Debug**: Debugging information
- **Info**: General informational messages (default minimum level)
- **Warn**: Warning messages for unusual situations
- **Error**: Error messages for recoverable errors
- **Critical**: Critical messages for severe unrecoverable errors

### 2. **Category Filtering**

Exclude entire categories from logging to reduce noise:

```cpp
Logger::setup(LogLevel::Debug, "app.log", {"server", "network"});
```

Messages logged with the "server" or "network" category will not appear.

### 3. **Multiple Outputs**

Log to both console (with colors) and files simultaneously:

- Console output with color coding for different severity levels
- Optional file output with automatic log rotation

### 4. **Source Location Tracking**

Automatically captures and displays:

- Filename and line number where the log was called
- Function name
- Configurable to show full path or just filename

### 5. **Thread Safety**

Safe to use from multiple threads without synchronization issues.

## Quick Start

### Basic Setup

```cpp
#include "logger/Logger.hpp"

int main() {
    // Initialize with defaults
    logger::Logger::setup();

    LOG_INFO("Application started");
    LOG_ERROR("An error occurred");

    return 0;
}
```

### Custom Configuration

```cpp
#include "logger/Logger.hpp"

int main() {
    // Setup with custom parameters
    logger::Logger::setup(
        logger::LogLevel::Debug,      // Minimum log level
        "app.log",                     // Log filename
        {"server", "network"}          // Excluded categories
    );

    LOG_INFO("Application started");
    LOG_INFO_CAT("client", "Client connected");      // VISIBLE
    LOG_INFO_CAT("server", "Server message");         // HIDDEN (excluded)

    logger::Logger::getInstance().shutdown();
    return 0;
}
```

## Documentation Structure

- **[Design Rationale](design-rationale.md)** - Why spdlog was chosen over other logging libraries
- **[Getting Started](getting-started.md)** - Quick setup guide and basic examples
- **[Configuration](configuration.md)** - Detailed settings and customization options
- **[Usage](usage.md)** - Comprehensive usage examples and best practices
- **[API Reference](api-reference.md)** - Complete API documentation

## Architecture

The Logger system consists of:

1. **Logger (Singleton)** - Main logger instance, manages initialization and logging
2. **Settings (Struct)** - Configuration object with all customizable parameters
3. **LogLevel (Enum)** - Available severity levels
4. **Macros** - Convenient logging macros for easy logging with automatic location info

## Macros

The logger provides convenient macros for different severity levels:

```cpp
LOG_TRACE(...)      // Trace level
LOG_DEBUG(...)      // Debug level
LOG_INFO(...)       // Info level
LOG_WARN(...)       // Warning level
LOG_ERROR(...)      // Error level
LOG_CRITICAL(...)   // Critical level
```

With categories:

```cpp
LOG_INFO_CAT(category, ...)
LOG_DEBUG_CAT(category, ...)
LOG_WARN_CAT(category, ...)
LOG_ERROR_CAT(category, ...)
LOG_CRITICAL_CAT(category, ...)
```

## License

The logger uses [spdlog](https://github.com/gabime/spdlog), which is distributed under the MIT License.
