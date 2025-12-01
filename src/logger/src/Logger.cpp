#include "logger/Logger.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/sink.h>
#include <iostream>
#include <cstring>
#include <algorithm>

namespace logger {

    bool Logger::isCategoryExcluded(const std::string& category) const {
        return std::find(m_excludedCategories.begin(), m_excludedCategories.end(), category)
            != m_excludedCategories.end();
    }

    const char* Logger::getFileName(const char* fullPath) {
        if (!fullPath) return fullPath;

        const char* lastSlash = std::strrchr(fullPath, '/');
        if (lastSlash) {
            return lastSlash + 1;
        }

        lastSlash = std::strrchr(fullPath, '\\');
        if (lastSlash) {
            return lastSlash + 1;
        }

        return fullPath;
    }

    Logger &Logger::getInstance() {
        static Logger instance;
        return instance;
    }

    void Logger::initialize(const Settings &settings) {
        try {
            std::vector<spdlog::sink_ptr> sinks;

            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(static_cast<spdlog::level::level_enum>(settings.minimumLevel));
            sinks.push_back(console_sink);

            if (settings.logFileName && settings.logFileName[0] != '\0') {
                spdlog::sink_ptr file_sink;

                if (settings.maxFileSize > 0 && settings.maxBackupCount > 0) {
                    file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                        settings.logFileName,
                        settings.maxFileSize,
                        settings.maxBackupCount
                    );
                } else {
                    file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                        settings.logFileName
                    );
                }

                file_sink->set_level(static_cast<spdlog::level::level_enum>(settings.minimumLevel));
                sinks.push_back(file_sink);
            }

            m_logger = std::make_shared<spdlog::logger>("R-Type", sinks.begin(), sinks.end());
            m_logger->set_level(static_cast<spdlog::level::level_enum>(settings.minimumLevel));

            std::string pattern;

            if (settings.showTimestamp) {
                pattern += settings.timestampFormat;
                pattern += " ";
            }

            if (settings.showThreadInfo) {
                pattern += "[%t] ";
            }

            pattern += "[%l] ";

            pattern += "%v";

            if (settings.showSourceLocation) {
                pattern += " (%@)";
            }

            m_logger->set_pattern(pattern);

            m_initialized = true;
            m_minimumLevel = settings.minimumLevel;
            m_showFullPath = settings.showFullPath;
            m_excludedCategories = settings.excludedCategories;

            LOG_INFO("Logger initialisé avec succès");
        } catch (const std::exception &e) {
            std::cerr << "Erreur lors de l'initialisation du logger: " << e.what() << std::endl;
            m_initialized = false;
        }
    }

    bool Logger::isInitialized() const {
        return m_initialized;
    }

    void Logger::setLogLevel(LogLevel level) {
        m_minimumLevel = level;
        if (m_logger) {
            m_logger->set_level(static_cast<spdlog::level::level_enum>(level));
        }
    }

    LogLevel Logger::getLogLevel() const {
        return m_minimumLevel;
    }

    void Logger::flush() {
        if (m_logger) {
            m_logger->flush();
        }
    }

    void Logger::shutdown() {
        if (m_logger) {
            m_logger->flush();
            m_logger.reset();
        }
        m_initialized = false;
    }

    Logger::~Logger() {
        shutdown();
    }

    void Logger::setup(
        LogLevel level,
        const char* filename,
        const std::vector<std::string> &excludedCategories,
        bool showFullPath,
        bool showTimestamp,
        bool showThreadInfo,
        bool showSourceLocation
    ) {
        Settings settings;
        settings.minimumLevel = level;
        settings.logFileName = filename;
        settings.excludedCategories = excludedCategories;
        settings.showFullPath = showFullPath;
        settings.showTimestamp = showTimestamp;
        settings.showThreadInfo = showThreadInfo;
        settings.showSourceLocation = showSourceLocation;

        getInstance().initialize(settings);
    }

}
