# Logger API Reference

## Enumerations

### LogLevel

Severity levels for log messages.

```cpp
enum class LogLevel {
    Trace = 0,    ///< Trace level (very verbose details)
    Debug = 1,    ///< Debug level (debugging information)
    Info = 2,     ///< Information level (general information)
    Warn = 3,     ///< Warning level (important warnings)
    Error = 4,    ///< Error level (errors)
    Critical = 5  ///< Critical level (critical errors)
};
```

**Usage:**

```cpp
logger::Logger::setup(logger::LogLevel::Debug);
logger::Logger::getInstance().setLogLevel(logger::LogLevel::Info);
```

---

## Structures

### Settings

Configuration structure for the logger.

```cpp
struct Settings {
    LogLevel minimumLevel = LogLevel::Info;
    bool showTimestamp = true;
    bool showThreadInfo = true;
    bool showSourceLocation = true;
    bool showFullPath = false;
    const char* timestampFormat = "[%Y-%m-%d %H:%M:%S.%f]";
    const char* logFileName = "";
    size_t maxFileSize = 0;
    size_t maxBackupCount = 0;
    std::vector<std::string> excludedCategories;
};
```

**Members:**

- `minimumLevel` - Minimum log level to display
- `showTimestamp` - Display timestamp in logs
- `showThreadInfo` - Display thread ID in logs
- `showSourceLocation` - Display file and line number
- `showFullPath` - Show full path or just filename
- `timestampFormat` - Strftime format string for timestamp
- `logFileName` - Log file path (empty = no file)
- `maxFileSize` - Max file size before rotation (0 = no rotation)
- `maxBackupCount` - Number of backup files to keep
- `excludedCategories` - Categories to exclude from logging

---

## Class: Logger

Singleton logger class with all logging functionality.

### Static Methods

#### getInstance()

```cpp
static Logger& getInstance();
```

Gets the unique logger instance (Singleton pattern).

**Returns:** Reference to the logger instance

**Example:**

```cpp
logger::Logger& logger = logger::Logger::getInstance();
```

---

#### setup()

```cpp
static void setup(
    LogLevel level = LogLevel::Info,
    const char* filename = "",
    const std::vector<std::string>& excludedCategories = {},
    bool showFullPath = false,
    bool showTimestamp = true,
    bool showThreadInfo = true,
    bool showSourceLocation = true
);
```

Convenience method to setup and initialize the logger with custom settings.

**Parameters:**

- `level` - Minimum log level (default: Info)
- `filename` - Log file path (default: "")
- `excludedCategories` - Categories to exclude (default: {})
- `showFullPath` - Show full file path (default: false)
- `showTimestamp` - Show timestamp (default: true)
- `showThreadInfo` - Show thread info (default: true)
- `showSourceLocation` - Show source location (default: true)

**Example:**

```cpp
logger::Logger::setup(logger::LogLevel::Debug, "app.log", {"verbose"});
```

---

#### getFileName()

```cpp
static const char* getFileName(const char* fullPath);
```

Extracts filename from a full path (used internally by macros).

**Parameters:**

- `fullPath` - Full file path

**Returns:** Pointer to filename (after last / or \)

**Example:**

```cpp
const char* name = logger::Logger::getFileName("/path/to/file.cpp");
// Returns: "file.cpp"
```

---

### Member Methods

#### initialize()

```cpp
void initialize(const Settings& settings);
```

Initializes the logger with the given settings.

**Parameters:**

- `settings` - Configuration object

**Example:**

```cpp
logger::Settings settings;
settings.minimumLevel = logger::LogLevel::Debug;
logger::Logger::getInstance().initialize(settings);
```

---

#### isInitialized()

```cpp
bool isInitialized() const;
```

Checks if the logger is currently initialized.

**Returns:** true if initialized, false otherwise

**Example:**

```cpp
if (logger::Logger::getInstance().isInitialized()) {
    LOG_INFO("Logger is ready");
}
```

---

#### setLogLevel()

```cpp
void setLogLevel(LogLevel level);
```

Changes the minimum log level at runtime.

**Parameters:**

- `level` - New minimum log level

**Example:**

```cpp
logger::Logger::getInstance().setLogLevel(logger::LogLevel::Trace);
```

---

#### getLogLevel()

```cpp
LogLevel getLogLevel() const;
```

Gets the current minimum log level.

**Returns:** Current log level

**Example:**

```cpp
if (logger::Logger::getInstance().getLogLevel() <= logger::LogLevel::Debug) {
    LOG_DEBUG("Debug info: {}", expensive_calculation());
}
```

---

#### flush()

```cpp
void flush();
```

Flushes all pending logs to disk and console.

**Example:**

```cpp
LOG_INFO("Important message");
logger::Logger::getInstance().flush();  // Ensure it's written
```

---

#### shutdown()

```cpp
void shutdown();
```

Shuts down the logger cleanly, flushing all pending logs.

**Example:**

```cpp
logger::Logger::getInstance().shutdown();
```

---

#### isCategoryExcluded()

```cpp
bool isCategoryExcluded(const std::string& category) const;
```

Checks if a category is excluded from logging.

**Parameters:**

- `category` - Category name to check

**Returns:** true if excluded, false otherwise

**Example:**

```cpp
if (!logger::Logger::getInstance().isCategoryExcluded("network")) {
    LOG_INFO_CAT("network", "Network message");
}
```

---

## Logging Macros

### Basic Macros

#### LOG_TRACE

```cpp
#define LOG_TRACE(...)
```

Logs a TRACE level message (most verbose).

**Example:**

```cpp
LOG_TRACE("Entering function with value: {}", value);
```

---

#### LOG_DEBUG

```cpp
#define LOG_DEBUG(...)
```

Logs a DEBUG level message.

**Example:**

```cpp
LOG_DEBUG("Variable x: {}", x);
```

---

#### LOG_INFO

```cpp
#define LOG_INFO(...)
```

Logs an INFO level message.

**Example:**

```cpp
LOG_INFO("Server started on port {}", port);
```

---

#### LOG_WARN

```cpp
#define LOG_WARN(...)
```

Logs a WARN level message.

**Example:**

```cpp
LOG_WARN("Memory usage: {}%", usage);
```

---

#### LOG_ERROR

```cpp
#define LOG_ERROR(...)
```

Logs an ERROR level message.

**Example:**

```cpp
LOG_ERROR("Connection failed: {}", error_msg);
```

---

#### LOG_CRITICAL

```cpp
#define LOG_CRITICAL(...)
```

Logs a CRITICAL level message.

**Example:**

```cpp
LOG_CRITICAL("System failure: {}", reason);
```

---

### Category Macros

#### LOG_TRACE_CAT

```cpp
#define LOG_TRACE_CAT(category, ...)
```

Logs a TRACE message with a category.

**Parameters:**

- `category` - Category name
- `...` - Message format and arguments

**Example:**

```cpp
LOG_TRACE_CAT("physics", "Collision check: {}", entity_id);
```

---

#### LOG_DEBUG_CAT

```cpp
#define LOG_DEBUG_CAT(category, ...)
```

Logs a DEBUG message with a category.

**Example:**

```cpp
LOG_DEBUG_CAT("network", "Packet received: {} bytes", size);
```

---

#### LOG_INFO_CAT

```cpp
#define LOG_INFO_CAT(category, ...)
```

Logs an INFO message with a category.

**Example:**

```cpp
LOG_INFO_CAT("gameplay", "Player {} connected", player_name);
```

---

#### LOG_WARN_CAT

```cpp
#define LOG_WARN_CAT(category, ...)
```

Logs a WARN message with a category.

**Example:**

```cpp
LOG_WARN_CAT("rendering", "GPU memory low: {}%", usage);
```

---

#### LOG_ERROR_CAT

```cpp
#define LOG_ERROR_CAT(category, ...)
```

Logs an ERROR message with a category.

**Example:**

```cpp
LOG_ERROR_CAT("physics", "Collision resolution failed");
```

---

#### LOG_CRITICAL_CAT

```cpp
#define LOG_CRITICAL_CAT(category, ...)
```

Logs a CRITICAL message with a category.

**Example:**

```cpp
LOG_CRITICAL_CAT("server", "Database connection lost");
```

---

## Utility Functions

### logLevelToString()

```cpp
constexpr const char* logLevelToString(LogLevel level) noexcept;
```

Converts a LogLevel to its string representation.

**Parameters:**

- `level` - Log level to convert

**Returns:** String representation ("TRACE", "DEBUG", "INFO", "WARN", "ERROR", "CRITICAL")

**Example:**

```cpp
std::cout << logLevelToString(LogLevel::Info);  // Output: "INFO"
```

---

## Thread Safety

All logger operations are **thread-safe** and can be safely called from multiple threads simultaneously.

**Example:**

```cpp
std::thread t1([]() { LOG_INFO("From thread 1"); });
std::thread t2([]() { LOG_INFO("From thread 2"); });
t1.join();
t2.join();
// Both messages logged correctly without race conditions
```

---

## Format String Syntax

The logger uses `std::format` compatible formatting:

| Format | Description | Example |
|--------|-------------|---------|
| `{}` | Default | `LOG_INFO("Value: {}", 42)` |
| `{:d}` | Decimal integer | `LOG_INFO("{:d}", 255)` |
| `{:x}` | Hexadecimal | `LOG_INFO("{:x}", 255)` → `ff` |
| `{:#x}` | Hex with prefix | `LOG_INFO("{:#x}", 255)` → `0xff` |
| `{:.2f}` | Float precision | `LOG_INFO("{:.2f}", 3.14159)` → `3.14` |
| `{:>10}` | Right align | `LOG_INFO("{:>10}", "hi")` → `"        hi"` |
| `{:<10}` | Left align | `LOG_INFO("{:<10}", "hi")` → `"hi        "` |

---

## Compilation

Include the logger header:

```cpp
#include "logger/Logger.hpp"
```

Link against the logger library in CMakeLists.txt:

```cmake
target_link_libraries(your_target logger)
```

---

## Error Handling

The logger is designed to be robust. If initialization fails:

```cpp
logger::Logger::setup(logger::LogLevel::Info, "invalid/path/file.log");
// Logger will handle the error gracefully and continue with console output
```

---

## Platform Support

- **Linux**: Full support
- **Windows**: Full support with Visual C++

---

## See Also

- [Getting Started](getting-started.md)
- [Configuration](configuration.md)
- [Usage Guide](usage.md)
- [spdlog Documentation](https://github.com/gabime/spdlog)
