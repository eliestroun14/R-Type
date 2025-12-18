#pragma once

#include <string>
#include <memory>
#include <vector>
#include <spdlog/logger.h>
#include <spdlog/common.h>
#include <spdlog/fmt/fmt.h>
#include "./LogLevel.hpp"
#include "./Settings.hpp"

namespace logger {

    /**
     * @class Logger
     * @brief Universal singleton logger based on spdlog
     *
     * Usage:
     * @code
     * Logger::getInstance().initialize(settings);
     * Logger::getInstance().info("Information message");
     * Logger::getInstance().error("Error message");
     * @endcode
     */
    class Logger {
    public:
        /**
         * @brief Gets the unique logger instance (Singleton)
         * @return Reference to the logger
         */
        static Logger &getInstance();

        /**
         * @brief Initializes the logger with the given parameters
         * @param settings Logger configuration
         */
        void initialize(const Settings &settings);

        /**
         * @brief Checks if the logger is initialized
         * @return true if initialized, false otherwise
         */
        bool isInitialized() const;

        /**
         * @brief Changes the minimum log level
         * @param level The new minimum level
         */
        void setLogLevel(LogLevel level);

        /**
         * @brief Gets the current minimum log level
         * @return The current minimum level
         */
        LogLevel getLogLevel() const;

        /**
         * @brief Flushes all pending logs
         */
        void flush();

        /**
         * @brief Shuts down the logger cleanly
         */
        void shutdown();

        /**
         * @brief Extracts the filename from the full path (public for macros)
         * @param fullPath Full path
         * @return Filename only
         */
        static const char* getFileName(const char* fullPath);

        /**
         * @brief Checks if a category is excluded
         * @param category The category to check
         * @return true if the category is excluded, false otherwise
         */
        bool isCategoryExcluded(const std::string& category) const;

        /**
         * @brief Sets up and initializes the logger with custom settings
         *
         * This is a convenience method that creates a Settings object and initializes
         * the logger in one call. All parameters are optional and use default values
         * if not provided (same defaults as Settings struct).
         *
         * Usage:
         * @code
         * Logger::setup(); // Uses all defaults
         * Logger::setup(LogLevel::Debug, "app.log", {"server"});
         * @endcode
         *
         * @param level Minimum log level (default: LogLevel::Info)
         * @param filename Log filename (default: "")
         * @param excludedCategories Categories to exclude (default: {})
         * @param showFullPath Show full file path (default: false)
         * @param showTimestamp Show timestamp (default: true)
         * @param showThreadInfo Show thread info (default: true)
         * @param showSourceLocation Show source location (default: true)
         */
        static void setup(
            LogLevel level = LogLevel::Info,
            const char* filename = "",
            const std::vector<std::string> &excludedCategories = {},
            bool showFullPath = false,
            bool showTimestamp = true,
            bool showThreadInfo = true,
            bool showSourceLocation = true
        );

        // Deletion of copy/move constructors
        Logger(const Logger&) = delete;
        Logger &operator=(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger &operator=(Logger&&) = delete;

        // Public access for macros
        std::shared_ptr<spdlog::logger> m_logger;
        bool m_showFullPath = false;
        bool m_initialized = false;
        LogLevel m_minimumLevel = LogLevel::Info;
        std::vector<std::string> m_excludedCategories;

    private:
        /**
         * @brief Private constructor (Singleton)
         */
        Logger() = default;

        /**
         * @brief Destructor
         */
        ~Logger();
    };

}

/**
 * @defgroup LoggingMacros Logging Macros
 * @brief Macros to record messages at different severity levels
 *
 * These macros automatically capture:
 * - The source file name (__FILE__)
 * - The line number (__LINE__)
 * - The function name (SPDLOG_FUNCTION)
 *
 * @note Macros must remain macros to capture location information
 * at the call point, not within the logger library.
 *
 * @{
 */

/**
 * @def LOG_TRACE
 * @brief Records a TRACE level message (most verbose)
 *
 * Logical equivalent (what the macro actually does):
 * @code
 * if (Logger::getInstance().isInitialized()) {
 *     spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION};
 *     if (!Logger::getInstance().m_showFullPath) {
 *         loc.filename = Logger::getFileName(loc.filename);
 *     }
 *     if (Logger::getInstance().getLogLevel() <= LogLevel::Trace) {
 *         Logger::getInstance().m_logger->log(loc, spdlog::level::trace, message);
 *     }
 * }
 * @endcode
 *
 * Usage:
 * @code
 * LOG_TRACE("Very verbose detail: {}", variable);
 * @endcode
 *
 * @param ... Message in spdlog format (std::format compatible)
 *
 * @see LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_CRITICAL
 */
#define LOG_TRACE(...) \
    do { \
        spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION}; \
        if (!::logger::Logger::getInstance().isInitialized()) break; \
        if (::logger::Logger::getInstance().getLogLevel() > ::logger::LogLevel::Trace) break; \
        if (!::logger::Logger::getInstance().m_showFullPath) \
            loc.filename = ::logger::Logger::getFileName(loc.filename); \
        ::logger::Logger::getInstance().m_logger->log(loc, spdlog::level::trace, __VA_ARGS__); \
    } while(0)

/**
 * @def LOG_DEBUG
 * @brief Records a DEBUG level message
 *
 * Logical equivalent (what the macro actually does):
 * @code
 * if (Logger::getInstance().isInitialized()) {
 *     spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION};
 *     if (!Logger::getInstance().m_showFullPath) {
 *         loc.filename = Logger::getFileName(loc.filename);
 *     }
 *     if (Logger::getInstance().getLogLevel() <= LogLevel::Debug) {
 *         Logger::getInstance().m_logger->log(loc, spdlog::level::debug, message);
 *     }
 * }
 * @endcode
 *
 * Used for detailed debugging information.
 *
 * Usage:
 * @code
 * LOG_DEBUG("Server state: {}", state);
 * @endcode
 *
 * @param ... Message in spdlog format (std::format compatible)
 *
 * @see LOG_TRACE, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_CRITICAL
 */
#define LOG_DEBUG(...) \
    do { \
        spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION}; \
        if (!::logger::Logger::getInstance().isInitialized()) break; \
        if (::logger::Logger::getInstance().getLogLevel() > ::logger::LogLevel::Debug) break; \
        if (!::logger::Logger::getInstance().m_showFullPath) \
            loc.filename = ::logger::Logger::getFileName(loc.filename); \
        ::logger::Logger::getInstance().m_logger->log(loc, spdlog::level::debug, __VA_ARGS__); \
    } while(0)

/**
 * @def LOG_INFO
 * @brief Records an information message (level displayed by default)
 *
 * Logical equivalent (what the macro actually does):
 * @code
 * if (Logger::getInstance().isInitialized()) {
 *     spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION};
 *     if (!Logger::getInstance().m_showFullPath) {
 *         loc.filename = Logger::getFileName(loc.filename);
 *     }
 *     if (Logger::getInstance().getLogLevel() <= LogLevel::Info) {
 *         Logger::getInstance().m_logger->log(loc, spdlog::level::info, message);
 *     }
 * }
 * @endcode
 *
 * This is the minimum level displayed by default. Used for normal program
 * events (startup, shutdown, user actions, etc).
 *
 * TRACE and DEBUG levels only display if you enable them manually.
 *
 * Usage:
 * @code
 * LOG_INFO("R-Type application started on port {}", 4242);
 * @endcode
 *
 * @param ... Message in spdlog format (std::format compatible)
 *
 * @see LOG_TRACE, LOG_DEBUG, LOG_WARN, LOG_ERROR, LOG_CRITICAL
 */
#define LOG_INFO(...) \
    do { \
        spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION}; \
        if (!::logger::Logger::getInstance().isInitialized()) break; \
        if (::logger::Logger::getInstance().getLogLevel() > ::logger::LogLevel::Info) break; \
        if (!::logger::Logger::getInstance().m_showFullPath) \
            loc.filename = ::logger::Logger::getFileName(loc.filename); \
        ::logger::Logger::getInstance().m_logger->log(loc, spdlog::level::info, __VA_ARGS__); \
    } while(0)

/**
 * @def LOG_WARN
 * @brief Records a warning message
 *
 * Logical equivalent (what the macro actually does):
 * @code
 * if (Logger::getInstance().isInitialized()) {
 *     spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION};
 *     if (!Logger::getInstance().m_showFullPath) {
 *         loc.filename = Logger::getFileName(loc.filename);
 *     }
 *     if (Logger::getInstance().getLogLevel() <= LogLevel::Warn) {
 *         Logger::getInstance().m_logger->log(loc, spdlog::level::warn, message);
 *     }
 * }
 * @endcode
 *
 * Used for unusual situations that are not errors.
 * The program continues but something deserves attention.
 *
 * Usage:
 * @code
 * LOG_WARN("High latency detected: {} ms", latency);
 * @endcode
 *
 * @param ... Message in spdlog format (std::format compatible)
 *
 * @see LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_ERROR, LOG_CRITICAL
 */
#define LOG_WARN(...) \
    do { \
        spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION}; \
        if (!::logger::Logger::getInstance().isInitialized()) break; \
        if (::logger::Logger::getInstance().getLogLevel() > ::logger::LogLevel::Warn) break; \
        if (!::logger::Logger::getInstance().m_showFullPath) \
            loc.filename = ::logger::Logger::getFileName(loc.filename); \
        ::logger::Logger::getInstance().m_logger->log(loc, spdlog::level::warn, __VA_ARGS__); \
    } while(0)

/**
 * @def LOG_ERROR
 * @brief Records an error message
 *
 * Logical equivalent (what the macro actually does):
 * @code
 * if (Logger::getInstance().isInitialized()) {
 *     spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION};
 *     if (!Logger::getInstance().m_showFullPath) {
 *         loc.filename = Logger::getFileName(loc.filename);
 *     }
 *     if (Logger::getInstance().getLogLevel() <= LogLevel::Error) {
 *         Logger::getInstance().m_logger->log(loc, spdlog::level::err, message);
 *     }
 * }
 * @endcode
 *
 * Used for recoverable errors. The program continues but
 * something went wrong.
 *
 * Usage:
 * @code
 * LOG_ERROR("Failed to connect to server: {}", error);
 * @endcode
 *
 * @param ... Message in spdlog format (std::format compatible)
 *
 * @see LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_CRITICAL
 */
#define LOG_ERROR(...) \
    do { \
        spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION}; \
        if (!::logger::Logger::getInstance().isInitialized()) break; \
        if (::logger::Logger::getInstance().getLogLevel() > ::logger::LogLevel::Error) break; \
        if (!::logger::Logger::getInstance().m_showFullPath) \
            loc.filename = ::logger::Logger::getFileName(loc.filename); \
        ::logger::Logger::getInstance().m_logger->log(loc, spdlog::level::err, __VA_ARGS__); \
    } while(0)

/**
 * @def LOG_CRITICAL
 * @brief Records a critical message (most severe)
 *
 * Logical equivalent (what the macro actually does):
 * @code
 * if (Logger::getInstance().isInitialized()) {
 *     spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION};
 *     if (!Logger::getInstance().m_showFullPath) {
 *         loc.filename = Logger::getFileName(loc.filename);
 *     }
 *     if (Logger::getInstance().getLogLevel() <= LogLevel::Critical) {
 *         Logger::getInstance().m_logger->log(loc, spdlog::level::critical, message);
 *     }
 * }
 * @endcode
 *
 * Used for irrecoverable errors. The program will likely
 * shut down or switch to degraded mode.
 *
 * Usage:
 * @code
 * LOG_CRITICAL("Critical system failure: shutdown imminent");
 * @endcode
 *
 * @param ... Message in spdlog format (std::format compatible)
 *
 * @see LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR
 */
#define LOG_CRITICAL(...) \
    do { \
        spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION}; \
        if (!::logger::Logger::getInstance().isInitialized()) break; \
        if (::logger::Logger::getInstance().getLogLevel() > ::logger::LogLevel::Critical) break; \
        if (!::logger::Logger::getInstance().m_showFullPath) \
            loc.filename = ::logger::Logger::getFileName(loc.filename); \
        ::logger::Logger::getInstance().m_logger->log(loc, spdlog::level::critical, __VA_ARGS__); \
    } while(0)

/**
 * @defgroup LoggingMacrosWithCategory Logging Macros with Category
 * @brief Macros to record messages with a category
 *
 * These macros allow you to classify logs by category (server, client, etc).
 * Excluded categories in Settings will not be displayed.
 *
 * @note Categories are defined at runtime and created automatically.
 *
 * @{
 */

/**
 * @def LOG_INFO_CAT
 * @brief Records an information message with a category
 *
 * Usage:
 * @code
 * LOG_INFO_CAT("server", "Server started on port {}", 4242);
 * @endcode
 *
 * If "server" is in the excluded categories, the message will not be displayed.
 *
 * @param category The log category (std::string)
 * @param ... Message in spdlog format
 *
 * @see LOG_DEBUG_CAT, LOG_WARN_CAT, LOG_ERROR_CAT, LOG_CRITICAL_CAT
 */
#define LOG_INFO_CAT(category, ...) \
    do { \
        if (::logger::Logger::getInstance().isCategoryExcluded(category)) break; \
        spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION}; \
        if (!::logger::Logger::getInstance().isInitialized()) break; \
        if (::logger::Logger::getInstance().getLogLevel() > ::logger::LogLevel::Info) break; \
        if (!::logger::Logger::getInstance().m_showFullPath) \
            loc.filename = ::logger::Logger::getFileName(loc.filename); \
        ::logger::Logger::getInstance().m_logger->log(loc, spdlog::level::info, "[{}] {}", category, fmt::format(__VA_ARGS__)); \
    } while(0)

/**
 * @def LOG_DEBUG_CAT
 * @brief Records a DEBUG message with a category
 *
 * @param category The log category (std::string)
 * @param ... Message in spdlog format
 */
#define LOG_DEBUG_CAT(category, ...) \
    do { \
        if (::logger::Logger::getInstance().isCategoryExcluded(category)) break; \
        spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION}; \
        if (!::logger::Logger::getInstance().isInitialized()) break; \
        if (::logger::Logger::getInstance().getLogLevel() > ::logger::LogLevel::Debug) break; \
        if (!::logger::Logger::getInstance().m_showFullPath) \
            loc.filename = ::logger::Logger::getFileName(loc.filename); \
        ::logger::Logger::getInstance().m_logger->log(loc, spdlog::level::debug, "[{}] {}", category, fmt::format(__VA_ARGS__)); \
    } while(0)

/**
 * @def LOG_WARN_CAT
 * @brief Records a warning message with a category
 *
 * @param category The log category (std::string)
 * @param ... Message in spdlog format
 */
#define LOG_WARN_CAT(category, ...) \
    do { \
        if (::logger::Logger::getInstance().isCategoryExcluded(category)) break; \
        spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION}; \
        if (!::logger::Logger::getInstance().isInitialized()) break; \
        if (::logger::Logger::getInstance().getLogLevel() > ::logger::LogLevel::Warn) break; \
        if (!::logger::Logger::getInstance().m_showFullPath) \
            loc.filename = ::logger::Logger::getFileName(loc.filename); \
        ::logger::Logger::getInstance().m_logger->log(loc, spdlog::level::warn, "[{}] {}", category, fmt::format(__VA_ARGS__)); \
    } while(0)

/**
 * @def LOG_ERROR_CAT
 * @brief Records an error message with a category
 *
 * @param category The log category (std::string)
 * @param ... Message in spdlog format
 */
#define LOG_ERROR_CAT(category, ...) \
    do { \
        if (::logger::Logger::getInstance().isCategoryExcluded(category)) break; \
        spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION}; \
        if (!::logger::Logger::getInstance().isInitialized()) break; \
        if (::logger::Logger::getInstance().getLogLevel() > ::logger::LogLevel::Error) break; \
        if (!::logger::Logger::getInstance().m_showFullPath) \
            loc.filename = ::logger::Logger::getFileName(loc.filename); \
        ::logger::Logger::getInstance().m_logger->log(loc, spdlog::level::err, "[{}] {}", category, fmt::format(__VA_ARGS__)); \
    } while(0)

/**
 * @def LOG_CRITICAL_CAT
 * @brief Records a critical message with a category
 *
 * @param category The log category (std::string)
 * @param ... Message in spdlog format
 */
#define LOG_CRITICAL_CAT(category, ...) \
    do { \
        if (::logger::Logger::getInstance().isCategoryExcluded(category)) break; \
        spdlog::source_loc loc{__FILE__, __LINE__, SPDLOG_FUNCTION}; \
        if (!::logger::Logger::getInstance().isInitialized()) break; \
        if (::logger::Logger::getInstance().getLogLevel() > ::logger::LogLevel::Critical) break; \
        if (!::logger::Logger::getInstance().m_showFullPath) \
            loc.filename = ::logger::Logger::getFileName(loc.filename); \
        ::logger::Logger::getInstance().m_logger->log(loc, spdlog::level::critical, "[{}] {}", category, fmt::format(__VA_ARGS__)); \
    } while(0)
