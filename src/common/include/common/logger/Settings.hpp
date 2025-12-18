#pragma once

#include <vector>
#include <string>

namespace logger {

    /**
     * @struct Settings
     * @brief Logger configuration
     */
    struct Settings {
        /// Minimum log level to display
        LogLevel minimumLevel = LogLevel::Info;

        /// Display timestamp in logs
        bool showTimestamp = true;

        /// Display thread information
        bool showThreadInfo = true;

        /// Display source code location (file:line)
        bool showSourceLocation = true;

        /// Display full file path (true) or just the name (false)
        bool showFullPath = false;

        /// Timestamp format (for spdlog)
        /// Default: "[%Y-%m-%d %H:%M:%S.%f]"
        const char* timestampFormat = "[%Y-%m-%d %H:%M:%S.%f]";

        /// Log filename (empty = no file)
        const char* logFileName = "";

        /// Maximum file size before rotation (0 = no rotation)
        size_t maxFileSize = 0;

        /// Number of log files to keep during rotation
        size_t maxBackupCount = 0;

        /// Categories to exclude (ex: "server", "client")
        /// If a category is in this list, logs from that category will not be displayed
        std::vector<std::string> excludedCategories;
    };

}
