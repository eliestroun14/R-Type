#pragma once

namespace logger {

    /**
     * @enum LogLevel
     * @brief Enumeration of available log levels
     */
    enum class LogLevel {
        Trace = 0,    ///< Trace level (very verbose details)
        Debug = 1,    ///< Debug level (debugging information)
        Info = 2,     ///< Information level (general information)
        Warn = 3,     ///< Warning level (important warnings)
        Error = 4,    ///< Error level (errors)
        Critical = 5  ///< Critical level (critical errors)
    };

    /**
     * @brief Converts a LogLevel to string
     * @param level The log level
     * @return String representation of the level
     */
    constexpr const char* logLevelToString(LogLevel level) noexcept {
        switch (level) {
            case LogLevel::Trace:
                return "TRACE";
            case LogLevel::Debug:
                return "DEBUG";
            case LogLevel::Info:
                return "INFO";
            case LogLevel::Warn:
                return "WARN";
            case LogLevel::Error:
                return "ERROR";
            case LogLevel::Critical:
                return "CRITICAL";
            default:
                return "UNKNOWN";
        }
    }

}
