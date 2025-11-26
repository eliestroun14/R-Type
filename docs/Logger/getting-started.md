# Getting Started with the Logger

## Installation

The logger is part of the R-Type project and is automatically built with the main project.

### Include Header

```cpp
#include "logger/Logger.hpp"
```

This header includes all necessary components: `Logger`, `Settings`, and `LogLevel`.

## Basic Setup

### Important: Choose ONE Setup Method

⚠️ **You can use EITHER the `setup()` method OR manual configuration, but NOT both together.**

Once you initialize the logger with one method, it's already configured and ready to use. You cannot call `setup()` and then modify settings after - the logger is already initialized!

---

### Method 1: Default Setup (Simplest)

Use all default settings with one line:

```cpp
#include "logger/Logger.hpp"

int main() {
    logger::Logger::setup();

    LOG_INFO("Hello World");

    return 0;
}
```

**Default settings:**

- Minimum level: `Info`
- No file output
- No excluded categories
- Shows timestamp, thread info, and source location
- Doesn't show full file path

**Best for:** Quick testing, prototyping, when defaults are sufficient

---

### Method 2: Custom Setup (Recommended for most cases)

Customize specific parameters in one call:

```cpp
#include "logger/Logger.hpp"

int main() {
    logger::Logger::setup(
        logger::LogLevel::Debug,           // Log level
        "game.log",                        // Log file
        {"network", "physics"}             // Excluded categories
    );

    LOG_DEBUG("Debug information");
    LOG_INFO_CAT("network", "Never appears");  // Excluded
    LOG_INFO_CAT("gameplay", "This appears");  // Not excluded

    return 0;
}
```

**Best for:** Custom configuration with basic needs

---

### Method 3: Manual Configuration (Full Control)

For complete control over all settings:

```cpp
#include "logger/Logger.hpp"

int main() {
    logger::Settings settings;
    settings.minimumLevel = logger::LogLevel::Debug;
    settings.logFileName = "game.log";
    settings.excludedCategories = {"network"};
    settings.showFullPath = true;
    settings.showTimestamp = true;
    settings.showThreadInfo = true;
    settings.showSourceLocation = true;
    settings.timestampFormat = "[%H:%M:%S]";  // Custom timestamp format
    settings.maxFileSize = 10 * 1024 * 1024;  // 10 MB
    settings.maxBackupCount = 5;              // Keep 5 backup files

    logger::Logger::getInstance().initialize(settings);

    LOG_INFO("Application started");

    return 0;
}
```

**Best for:** Advanced configuration, production environments, specific requirements

---

### Comparison Table

| Feature | Method 1 | Method 2 | Method 3 |
|---------|----------|----------|----------|
| **Setup time** | ⚡ Instant | ⚡ Fast | ⚡ Fast |
| **Code simplicity** | ✅ Minimal | ✅ Simple | ⚠️ More verbose |
| **Customization** | ❌ None | ✅ Good | ✅ Complete |
| **Control** | ❌ None | ✅ Moderate | ✅ Full |
| **Use case** | Testing | Most projects | Advanced/Production |

---

### Common Mistake ❌

**DON'T mix setup methods:**

```cpp
// ❌ WRONG - Will cause errors!
logger::Logger::setup(LogLevel::Debug, "app.log");
settings.timestampFormat = "[%H:%M:%S]";  // ERROR: settings doesn't exist!
```

**DO choose one method:**

```cpp
// ✅ CORRECT - Use setup() only
logger::Logger::setup(LogLevel::Debug, "app.log");

// OR ✅ CORRECT - Use manual configuration
logger::Settings settings;
settings.minimumLevel = LogLevel::Debug;
settings.logFileName = "app.log";
settings.timestampFormat = "[%H:%M:%S]";
logger::Logger::getInstance().initialize(settings);
```

---

## Basic Logging

### Log at Different Levels

```cpp
LOG_TRACE("Very detailed information");
LOG_DEBUG("Debug information");
LOG_INFO("General information");
LOG_WARN("Warning message");
LOG_ERROR("Error message");
LOG_CRITICAL("Critical error");
```

### Logging with Categories

```cpp
LOG_INFO_CAT("client", "Client connected");
LOG_WARN_CAT("server", "High memory usage");
LOG_ERROR_CAT("physics", "Collision error");
```

### Formatted Logging

Like `printf` and `std::format`:

```cpp
int port = 4242;
std::string player = "Alice";

LOG_INFO("Server listening on port {}", port);
LOG_INFO_CAT("client", "Player {} joined", player);
LOG_ERROR("Failed to connect to server after {} attempts", retries);
```

## Cleanup

Always shutdown the logger when done:

```cpp
logger::Logger::getInstance().shutdown();
```

Or in RAII style:

```cpp
class Game {
public:
    Game() {
        logger::Logger::setup(logger::LogLevel::Debug, "game.log");
    }

    ~Game() {
        logger::Logger::getInstance().shutdown();
    }
};

int main() {
    Game game;
    // ... game code ...
    return 0;  // Logger automatically shut down
}
```

## Example: Complete Application

```cpp
#include "logger/Logger.hpp"
#include <thread>
#include <chrono>

int main() {
    // Setup logger
    logger::Logger::setup(
        logger::LogLevel::Debug,
        "app.log",
        {"verbose_subsystem"}
    );

    LOG_INFO("Application started");

    // Simulate work from different subsystems
    LOG_DEBUG("Initializing game engine");
    LOG_INFO_CAT("client", "Client initialized");
    LOG_INFO_CAT("server", "Server listening");

    // This will be excluded
    LOG_DEBUG_CAT("verbose_subsystem", "This won't appear");

    // This will appear
    LOG_INFO_CAT("gameplay", "Game started");

    LOG_WARN("Low memory warning");

    // Flush any pending logs
    logger::Logger::getInstance().flush();

    // Shutdown
    logger::Logger::getInstance().shutdown();

    return 0;
}
```

## Next Steps

- See [Configuration](configuration.md) for all available settings
- See [Usage](usage.md) for advanced usage patterns
- See [API Reference](api-reference.md) for complete API documentation
