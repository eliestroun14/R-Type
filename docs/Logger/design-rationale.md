# Design Rationale: Why spdlog?

## Overview

The Logger system in R-Type is built on **spdlog** instead of other C++ logging libraries. This document explains the architectural and practical reasons behind this choice.

## Requirements Analysis

Before selecting spdlog, we identified the following requirements for the R-Type project:

### Functional Requirements

- **Multiple log levels** (Trace, Debug, Info, Warning, Error, Critical)
- **Dual output** (console + file logging)
- **Source location tracking** (file, line, function)
- **Thread-safe operation** (multiplayer game server)
- **Automatic log rotation** (prevent disk space issues)
- **Format flexibility** (timestamps, thread info, custom patterns)
- **Formatted output** (modern C++ formatting support)

### Non-Functional Requirements

- **High performance** (minimal overhead in game loop)
- **Header-only option** (easy integration)
- **Easy integration** (CPM package manager support)
- **Active maintenance** (security updates, bug fixes)
- **Minimal dependencies** (keep build simple)
- **MIT license compatibility** (commercial friendly)

## Comparison with Alternatives

### 1. **spdlog** ✅ (Selected)

**Pros:**

- **Extreme performance**: Lock-free queue design, asynchronous logging option
- **Rich formatting**: Pattern-based formatting with `{fmt}` style placeholders
- **Selective output**: Multiple sinks (console, file, rotating file)
- **Built-in source location**: Automatic file/line/function tracking
- **Thread-safe by default**: Lock-free internal design for high concurrency
- **Header-only variant**: Easy header inclusion without compilation
- **Log rotation**: Automatic file rotation by size and time
- **Color support**: Automatic ANSI color codes for console output
- **Well-documented**: Comprehensive documentation and examples
- **CPM compatible**: Easy dependency management
- **MIT License**: Commercial-friendly license

**Cons:**

- Large header file size (increases compilation time)
- Multiple sink configurations can be complex for beginners

**Why chosen for R-Type:**

- Performance is critical in game development
- Network synchronization requires thread-safe logging
- Server-client architecture benefits from multiple output sinks
- Easy integration via CPM aligns with project infrastructure

---

### 2. **Boost.Log** ❌ (Rejected)

**Pros:**

- Extremely flexible and powerful
- Part of established Boost library ecosystem
- Good documentation

**Cons:**

- **Heavy dependency**: Massive binary bloat (Boost adds 100+ MB)
- **Slow compilation**: Significant compilation time overhead
- **Overkill for requirements**: Too complex for game logging needs
- **CPM unfriendly**: Not well-supported by CPM package manager
- **Complex API**: Steep learning curve for standard logging tasks

**Why rejected:**

- R-Type build system uses CPM (lightweight dependency management)
- Game projects prioritize fast build times
- Unnecessary complexity for core logging features needed

---

### 3. **glog (Google Logging)** ❌ (Rejected)

**Pros:**

- Simple, clean API
- Good for application-level logging
- Reasonable performance

**Cons:**

- **Limited features**: No built-in color support
- **Poor Windows support**: Primarily designed for Linux/Unix
- **Minimal formatting options**: Limited pattern customization
- **Header requirements**: Complex header setup
- **No modern formatting**: Lacks C++20 formatting support

**Why rejected:**

- R-Type must support cross-platform development (client can run on Windows)
- Less active maintenance compared to spdlog
- Lacks modern C++ formatting capabilities needed for flexible logging

---

### 4. **log4cxx** ❌ (Rejected)

**Pros:**

- Industry-standard (based on Java's log4j)
- Mature and stable
- Widely used in enterprise

**Cons:**

- **Java-style overhead**: Designed for JVM, awkward in C++
- **External dependencies**: Requires APR (Apache Portable Runtime)
- **Poor modern C++ support**: Doesn't leverage C++17/20 features
- **Slower**: Performance not optimized for game loops
- **Maintenance decline**: Less active development

**Why rejected:**

- Performance overhead incompatible with game requirements
- Unnecessary external dependencies
- Better alternatives exist for modern C++

---

### 5. **Plog** ❌ (Rejected)

**Pros:**

- Header-only
- Very lightweight
- Simple API

**Cons:**

- **Limited features**: Minimal configuration options
- **No async logging**: Blocks thread during I/O
- **Poor thread safety**: Not optimized for high-concurrency scenarios
- **No category filtering**: Can't selectively disable log categories
- **Weak documentation**: Limited examples and guides

**Why rejected:**

- Thread synchronization bottleneck in multiplayer server
- Lack of category filtering problematic for debugging
- Limited feature set compared to actual needs

---

### 6. **Easylogging++** ❌ (Rejected)

**Pros:**

- Single header file
- Simple configuration
- Good color support

**Cons:**

- **Poor performance**: Performance not optimized for critical paths
- **Limited async options**: Blocks execution during I/O
- **No built-in rotation**: Manual file management required
- **Thread safety concerns**: Synchronization bottlenecks
- **Maintenance unclear**: Inconsistent update schedule

**Why rejected:**

- Performance concerns in game loop context
- No automatic log rotation (manual management burden)
- Thread safety not guaranteed under high contention

---

## spdlog: Detailed Advantages

### 1. **Performance**

spdlog uses a **lock-free queue design** for asynchronous logging:

```
Game Thread → Lock-free Queue → Logger Thread → Disk/Console
                     ↓
              Non-blocking operation
              Minimal impact on frame time
```

**Benchmark context:**

- Synchronous logging: ~100 nanoseconds per call
- Async logging: ~10 nanoseconds per call
- Alternative libraries: 1-5 microseconds per call

For a 60 FPS game (16.6ms frame budget), thousands of logs with spdlog add negligible overhead.

### 2. **Header-Only Flexibility**

spdlog offers two integration options:

- **Header-only mode**: Include and compile (no separate build needed)
- **Compiled mode**: Precompiled library (faster compilation in large projects)

R-Type uses header-only with CPM for simplicity.

### 3. **License Alignment**

spdlog uses **MIT License**, which:

- ✅ Allows commercial use (important for game distribution)
- ✅ Allows modifications

### 4. **Ecosystem Integration**

spdlog is the standard choice in modern game engines:

- **Unreal Engine**: Uses similar logging patterns
- **Godot**: Uses spdlog-like architecture
- **Game dev community**: De facto standard for C++ game logging

## Design Decisions Based on spdlog

### 1. **Category Filtering (R-Type Wrapper Enhancement)**

The R-Type Logger implementation adds **category filtering** on top of spdlog:

```cpp
// R-Type wrapper adds category filtering - NOT a spdlog feature
Logger::setup(LogLevel::Info, "game.log", {"network", "physics"});
```

**How it works:**

- Categories are stored in a filter set within the R-Type wrapper
- When `LOG_*_CAT(category, message)` is called, the wrapper checks if the category is in the exclusion list
- If excluded, the log is filtered out before reaching spdlog
- This is a **design decision** by R-Type to provide fine-grained control over logging noise

**Why this matters:**

- spdlog provides the **foundation** (formatting, output, performance)
- R-Type adds **semantic organization** (categories as a conceptual layer)
- This separation allows both flexibility and clean API

### 2. **Async Logging Option**

While implemented in wrapper, spdlog provides native async queue:

```cpp
auto async_file = std::make_shared<spdlog::sinks::async_file_sink_mt>(...);
```

Allows performance-critical sections to use async logging without framework changes.

### 3. **Thread-Safe Singleton**

spdlog's `spdlog::details::atomic` provides lock-free singleton pattern:

```cpp
// getInstance() is lock-free thanks to spdlog's atomic implementation
Logger& Logger::getInstance() { ... }
```

## Conclusion

**spdlog was selected because it uniquely satisfies R-Type's requirements:**

1. ✅ **Performance-first design** (critical for game development)
2. ✅ **Feature-complete** (all needed capabilities present)
3. ✅ **Easy integration** (header-only + CPM support)
4. ✅ **Thread-safe by default** (multiplayer server needs)
5. ✅ **Active maintenance** (security and feature updates)
6. ✅ **Minimal footprint** (no unnecessary dependencies)

### Alternatives Rejected Because

- **Boost.Log**: Too heavy, too slow to compile, unnecessary complexity
- **glog**: Missing critical features (color, filtering), platform limitations
- **log4cxx**: Performance concerns, Java-style overhead, declining maintenance
- **Plog**: Thread safety concerns, no async, minimal feature set
- **Easylogging++**: Performance issues, no auto-rotation, maintenance unclear

spdlog provides the optimal balance between **power, simplicity, and performance** for R-Type's game engine logging requirements.
