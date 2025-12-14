# Logger Usage Guide

## Logging Basics

### Log Levels

The logger provides six severity levels for different types of messages:

```cpp
LOG_TRACE("Very detailed information");      // Most verbose
LOG_DEBUG("Debug information");              // Debugging details
LOG_INFO("General information");             // Normal operations
LOG_WARN("Warning message");                 // Unusual situations
LOG_ERROR("Error message");                  // Recoverable errors
LOG_CRITICAL("Critical error");              // Severe unrecoverable errors
```

### When to Use Each Level

| Level | Use Case | Example |
|-------|----------|---------|
| **Trace** | Extremely detailed debugging | Variable values, loop iterations |
| **Debug** | Debugging information | Function entry/exit, state changes |
| **Info** | Normal operations | Application startup, major events |
| **Warn** | Unusual situations | High latency, unusual input |
| **Error** | Recoverable errors | Failed connections, invalid input |
| **Critical** | Severe errors | System failure, crash imminent |

### Basic Logging

```cpp
LOG_INFO("Application started");
LOG_ERROR("Connection failed");
LOG_WARN("Memory usage is high");
```

## Formatted Output

The logger supports `std::format` style formatting:

### Numeric Values

```cpp
int port = 4242;
int fps = 60;

LOG_INFO("Server listening on port {}", port);
LOG_DEBUG("FPS: {}", fps);
```

### String Values

```cpp
std::string player_name = "Alice";
std::string status = "connected";

LOG_INFO("Player {} is {}", player_name, status);
```

### Multiple Values

```cpp
int x = 100, y = 200;
float health = 75.5;

LOG_DEBUG("Position: ({}, {})", x, y);
LOG_INFO("Health: {:.1f}%", health);
```

### Complex Formatting

```cpp
// Hex numbers
int error_code = 255;
LOG_ERROR("Error code: {:#x}", error_code);  // 0xff

// Floating point precision
double pi = 3.14159265;
LOG_INFO("Pi: {:.2f}", pi);  // 3.14

// Padding and alignment
std::string name = "Player";
LOG_INFO("Name: {:>10}", name);  // Padded to 10 chars
```

## Categories

Categories allow you to group related messages and selectively exclude them.

### Using Categories

```cpp
LOG_INFO_CAT("category", "Message");
LOG_DEBUG_CAT("physics", "Collision detected");
LOG_WARN_CAT("network", "High latency");
LOG_ERROR_CAT("rendering", "Shader compilation failed");
```

### Excluding Categories

Exclude categories during setup:

```cpp
logger::Logger::setup(
    logger::LogLevel::Debug,
    "app.log",
    {"verbose", "debug_network"}
);

LOG_INFO_CAT("verbose", "Won't appear");        // Excluded
LOG_INFO_CAT("gameplay", "Will appear");        // Not excluded
```

### Common Categories

Suggest using consistent category names:

```cpp
// Client-side
LOG_INFO_CAT("client", "...");
LOG_INFO_CAT("input", "...");
LOG_INFO_CAT("rendering", "...");

// Server-side
LOG_INFO_CAT("server", "...");
LOG_INFO_CAT("network", "...");
LOG_INFO_CAT("game_logic", "...");

// Common
LOG_INFO_CAT("physics", "...");
LOG_INFO_CAT("audio", "...");
```

## Practical Examples

### Server Logging

```cpp
#include "logger/Logger.hpp"

class GameServer {
public:
    GameServer() {
        logger::Logger::setup(
            logger::LogLevel::Info,
            "server.log",
            {"verbose_network"}
        );
    }

    void start(int port) {
        LOG_INFO("Starting game server on port {}", port);
        if (!bind_socket(port)) {
            LOG_CRITICAL("Failed to bind socket");
            return;
        }
        LOG_INFO_CAT("server", "Server ready to accept connections");
    }

    void handle_client_connect(const std::string& ip) {
        LOG_INFO_CAT("network", "Client connected from {}", ip);
    }

    void handle_game_event(const std::string& event) {
        LOG_DEBUG_CAT("game_logic", "Event: {}", event);
    }
};
```

### Client Logging

```cpp
#include "logger/Logger.hpp"

class GameClient {
public:
    GameClient() {
        logger::Logger::setup(logger::LogLevel::Debug, "client.log");
    }

    void connect(const std::string& server) {
        LOG_INFO("Connecting to server: {}", server);
        if (!connect_socket(server)) {
            LOG_ERROR("Connection failed");
            return;
        }
        LOG_INFO_CAT("client", "Connected successfully");
    }

    void render_frame(int frame_num) {
        LOG_TRACE_CAT("rendering", "Rendering frame {}", frame_num);
    }

    void handle_input(const std::string& input) {
        LOG_DEBUG_CAT("input", "Input received: {}", input);
    }
};
```

### Physics Engine

```cpp
class PhysicsEngine {
public:
    void update(float delta_time) {
        LOG_TRACE_CAT("physics", "Updating physics (delta: {}s)", delta_time);

        for (auto& body : bodies) {
            if (check_collision(body)) {
                LOG_DEBUG_CAT("physics", "Collision detected: {}", body.id);
            }
        }
    }
};
```

### Network Communication

```cpp
class PacketManager {
public:
    void send_packet(const std::string& data, const std::string& dest) {
        LOG_TRACE_CAT("verbose_network", "Sending {} bytes to {}",
                     data.size(), dest);

        if (!send(data)) {
            LOG_ERROR_CAT("network", "Failed to send packet");
            return;
        }
        LOG_DEBUG_CAT("network", "Packet sent successfully");
    }

    void receive_packet(const std::string& data) {
        LOG_TRACE_CAT("verbose_network", "Received {} bytes", data.size());
        LOG_INFO_CAT("network", "Packet received");
    }
};
```

## Advanced Patterns

### Conditional Logging

```cpp
if (logger::Logger::getInstance().getLogLevel() <= logger::LogLevel::Debug) {
    LOG_DEBUG("Expensive computation: {}", expensive_calculation());
}
```

### Error Handling with Logging

```cpp
try {
    perform_operation();
} catch (const std::exception& e) {
    LOG_ERROR("Operation failed: {}", e.what());
}
```

### Performance-Critical Sections

```cpp
void update_game_state() {
    LOG_TRACE_CAT("verbose", "Update started");

    // Critical section - minimal logging
    for (int i = 0; i < 1000000; ++i) {
        // Don't log in tight loops!
        process_entity(entities[i]);
    }

    LOG_TRACE_CAT("verbose", "Update finished");
}
```

### Debugging State

```cpp
class Player {
    void debug_log() {
        LOG_DEBUG("Player state: HP={}, Pos=({},{}), Vel=({},{})",
                 health, pos.x, pos.y, vel.x, vel.y);
    }
};
```

### Startup/Shutdown Logging

```cpp
int main() {
    logger::Logger::setup(logger::LogLevel::Debug, "app.log");

    LOG_INFO("=== Application Starting ===");
    LOG_INFO("Version: 1.0.0");
    LOG_INFO("Platform: Linux");
    LOG_DEBUG("Build time: {}", __TIMESTAMP__);

    // Application code

    LOG_INFO("=== Application Shutting Down ===");
    logger::Logger::getInstance().shutdown();

    return 0;
}
```

## Performance Considerations

### Use Appropriate Log Levels

```cpp
// DON'T: Every frame
for (int i = 0; i < entities.size(); ++i) {
    LOG_INFO("Processing entity {}", i);  // Too much output
}

// DO: Debug level for frame-by-frame logging
LOG_DEBUG("Processing {} entities", entities.size());
```

### Avoid Expensive Computations in Logs

```cpp
// DON'T
LOG_DEBUG("Result: {}", expensive_function());

// DO
if (logger::Logger::getInstance().getLogLevel() <= logger::LogLevel::Debug) {
    LOG_DEBUG("Result: {}", expensive_function());
}
```

### Batch Related Messages

```cpp
// DON'T: Multiple separate logs
for (const auto& event : events) {
    LOG_INFO("Event: {}", event.type);
}

// DO: Single summary log
LOG_DEBUG("Processing {} events", events.size());
```

## Debugging Tips

### Find Intermittent Issues

```cpp
// Use high verbosity temporarily
logger::Logger::getInstance().setLogLevel(logger::LogLevel::Trace);

// Run with excluded categories empty to see everything
logger::Logger::setup(logger::LogLevel::Trace, "debug.log", {});
```

### Monitor Performance

```cpp
auto start = std::chrono::high_resolution_clock::now();
// ... code to measure ...
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
LOG_INFO("Operation took {} ms", duration.count());
```

### Trace Execution Flow

```cpp
void critical_function() {
    LOG_TRACE("critical_function: Entry");

    if (condition1) {
        LOG_TRACE("critical_function: Taking path A");
    } else {
        LOG_TRACE("critical_function: Taking path B");
    }

    LOG_TRACE("critical_function: Exit");
}
```

## Common Mistakes

### ❌ Don't Log in Tight Loops

```cpp
// Bad: Creates enormous log files
for (int i = 0; i < 1000000; ++i) {
    LOG_INFO("Processing item {}", i);
}
```

### ❌ Don't Use Multiple Loggers

```cpp
// Bad: Use the singleton
logger::Logger special_logger;  // Don't do this!

// Good: Use the singleton
logger::Logger::getInstance().setLogLevel(logger::LogLevel::Debug);
```

### ❌ Don't Forget to Shutdown

```cpp
// Bad: Log file not flushed
int main() {
    logger::Logger::setup();
    LOG_INFO("Running");
    return 0;  // Logger not shut down properly
}

// Good: Shutdown explicitly
int main() {
    logger::Logger::setup();
    LOG_INFO("Running");
    logger::Logger::getInstance().shutdown();
    return 0;
}
```

## Next Steps

- See [Configuration](configuration.md) for detailed settings
- See [API Reference](api-reference.md) for complete API
- See [Getting Started](getting-started.md) for basic setup
