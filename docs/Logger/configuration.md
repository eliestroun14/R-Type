# Logger Configuration

## Settings Structure

The logger is configured through the `Settings` struct, which contains all customizable parameters.

### Creating Settings

```cpp
logger::Settings settings;
// Modify as needed
settings.minimumLevel = logger::LogLevel::Debug;
logger::Logger::getInstance().initialize(settings);
```

Or use the convenient `setup()` method:

```cpp
logger::Logger::setup(
    logger::LogLevel::Debug,
    "app.log",
    {"category1", "category2"}
);
```

## Configuration Parameters

### minimumLevel

**Type:** `LogLevel`
**Default:** `LogLevel::Info`

Sets the minimum severity level for messages to be logged.

**Values:**

- `LogLevel::Trace` - Show all messages
- `LogLevel::Debug` - Debug and above
- `LogLevel::Info` - Info and above (default)
- `LogLevel::Warn` - Warnings and above
- `LogLevel::Error` - Errors and above
- `LogLevel::Critical` - Only critical messages

**Example:**

```cpp
settings.minimumLevel = logger::LogLevel::Debug;
// Messages at Trace level will not be shown
// Messages at Debug, Info, Warn, Error, Critical will be shown
```

### showTimestamp

**Type:** `bool`
**Default:** `true`

Display timestamp in log messages.

**Format:** `[YYYY-MM-DD HH:MM:SS.ffffff]`

**Example:**

```cpp
settings.showTimestamp = false;
// Output: [info] Message
// Instead of: [2025-11-26 10:30:45.123456] [info] Message
```

### showThreadInfo

**Type:** `bool`
**Default:** `true`

Display thread ID in log messages.

**Example:**

```cpp
settings.showThreadInfo = false;
// Output: [info] Message
// Instead of: [123456] [info] Message
```

### showSourceLocation

**Type:** `bool`
**Default:** `true`

Display source code location (filename and line number) in log messages.

**Example:**

```cpp
settings.showSourceLocation = false;
// Output: [info] Message
// Instead of: [info] Message (main.cpp:42)
```

### showFullPath

**Type:** `bool`
**Default:** `false`

When `showSourceLocation` is true, display the full file path or just the filename.

**Example:**

```cpp
settings.showFullPath = true;
// Output: (main.cpp:42) vs (/path/to/main.cpp:42)
```

### timestampFormat

**Type:** `const char*`
**Default:** `"[%Y-%m-%d %H:%M:%S.%f]"`

Customize the timestamp format using strftime format specifiers.

**Common specifiers:**

- `%Y` - Year (4 digits)
- `%m` - Month (01-12)
- `%d` - Day (01-31)
- `%H` - Hour (00-23)
- `%M` - Minute (00-59)
- `%S` - Second (00-59)
- `%f` - Microseconds (000000-999999)
- `%T` - Time (equivalent to %H:%M:%S)

**Examples:**

```cpp
settings.timestampFormat = "[%H:%M:%S]";          // [10:30:45]
settings.timestampFormat = "[%m/%d %H:%M]";       // [11/26 10:30]
settings.timestampFormat = "[%Y-%m-%d]";          // [2025-11-26]
```

### logFileName

**Type:** `const char*`
**Default:** `""`

Log filename for file output. Empty string disables file logging.

**Example:**

```cpp
settings.logFileName = "app.log";     // Enable file logging
settings.logFileName = "";            // Disable file logging
```

**Note:** Logs are always written to console regardless of this setting.

### maxFileSize

**Type:** `size_t`
**Default:** `0`

Maximum file size in bytes before log rotation occurs. `0` disables rotation.

**Example:**

```cpp
settings.maxFileSize = 10 * 1024 * 1024;  // 10 MB
// When the log file reaches 10 MB, it will be rotated
```

### maxBackupCount

**Type:** `size_t`
**Default:** `0`

Number of backup log files to keep when rotating. Only used when `maxFileSize > 0`.

**Example:**

```cpp
settings.maxFileSize = 10 * 1024 * 1024;  // 10 MB
settings.maxBackupCount = 5;               // Keep 5 backup files
// Files: app.log, app.log.1, app.log.2, app.log.3, app.log.4, app.log.5
```

### excludedCategories

**Type:** `std::vector<std::string>`
**Default:** `{}`

Categories to exclude from logging. Messages logged with excluded categories will not be displayed.

**Example:**

```cpp
settings.excludedCategories = {"verbose", "network", "physics"};

LOG_INFO_CAT("verbose", "This won't appear");  // Excluded
LOG_INFO_CAT("gameplay", "This will appear");   // Not excluded
```

## Runtime Configuration Changes

### Change Log Level

```cpp
logger::Logger::getInstance().setLogLevel(logger::LogLevel::Debug);
```

### Get Current Log Level

```cpp
logger::LogLevel current = logger::Logger::getInstance().getLogLevel();
```

### Flush Pending Logs

```cpp
logger::Logger::getInstance().flush();
```

### Check if Category is Excluded

```cpp
bool excluded = logger::Logger::getInstance().isCategoryExcluded("network");
```

## Best Practices

1. **Set log level appropriately** - Higher levels (Error, Critical) reduce noise
2. **Use categories wisely** - Group related messages with categories
3. **Rotate large files** - Set `maxFileSize` for long-running applications
4. **Exclude verbose categories** - Use `excludedCategories` to reduce output
5. **Customize timestamps** - Use timestamps appropriate for your use case
6. **Test configuration** - Verify your setup produces expected output
