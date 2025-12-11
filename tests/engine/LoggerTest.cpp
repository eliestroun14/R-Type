#include <gtest/gtest.h>
#include <spdlog/sinks/null_sink.h>
#include <filesystem>
#include <fstream>
#include <engine/utils/Logger.hpp>

namespace fs = std::filesystem;

/**
 * @class LoggerTest
 * @brief Test suite for the Logger class
 *
 * Tests cover:
 * - Singleton pattern
 * - Initialization
 * - Log levels
 * - Category exclusion
 * - File operations
 * - Filename extraction
 */
class LoggerTest : public ::testing::Test {
protected:
    /**
     * @brief Set up test fixtures before each test
     */
    void SetUp() override {
        // Clean up any previous logger state
        logger::Logger::getInstance().shutdown();
    }

    /**
     * @brief Tear down test fixtures after each test
     */
    void TearDown() override {
        // Clean up after each test
        logger::Logger::getInstance().shutdown();

        // Remove temporary log files if they were created
        const char* test_files[] = {"test.log", "test_rotation.log", "test_rotation.log.1"};
        for (const auto* file : test_files) {
            if (fs::exists(file)) {
                fs::remove(file);
            }
        }
    }
};

/**
 * @test SingletonPattern
 * @brief Verify that Logger follows the singleton pattern
 */
TEST_F(LoggerTest, SingletonPattern) {
    logger::Logger& instance1 = logger::Logger::getInstance();
    logger::Logger& instance2 = logger::Logger::getInstance();

    // Both should reference the same object
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @test InitializationDefault
 * @brief Verify logger initialization with default settings
 */
TEST_F(LoggerTest, InitializationDefault) {
    logger::Settings settings;
    settings.minimumLevel = logger::LogLevel::Info;

    logger::Logger::getInstance().initialize(settings);

    EXPECT_TRUE(logger::Logger::getInstance().isInitialized());
    EXPECT_EQ(logger::Logger::getInstance().getLogLevel(), logger::LogLevel::Info);
}

/**
 * @test InitializationWithSetup
 * @brief Verify logger initialization using the setup method
 */
TEST_F(LoggerTest, InitializationWithSetup) {
    logger::Logger::setup(logger::LogLevel::Debug);

    EXPECT_TRUE(logger::Logger::getInstance().isInitialized());
    EXPECT_EQ(logger::Logger::getInstance().getLogLevel(), logger::LogLevel::Debug);
}

/**
 * @test IsInitializedFalseByDefault
 * @brief Verify logger is not initialized by default
 */
TEST_F(LoggerTest, IsInitializedFalseByDefault) {
    EXPECT_FALSE(logger::Logger::getInstance().isInitialized());
}

/**
 * @test SetLogLevel
 * @brief Verify setting and getting log levels
 */
TEST_F(LoggerTest, SetLogLevel) {
    logger::Logger::setup(logger::LogLevel::Info);

    // Test all log levels
    const logger::LogLevel levels[] = {
        logger::LogLevel::Trace,
        logger::LogLevel::Debug,
        logger::LogLevel::Info,
        logger::LogLevel::Warn,
        logger::LogLevel::Error,
        logger::LogLevel::Critical
    };

    for (logger::LogLevel level : levels) {
        logger::Logger::getInstance().setLogLevel(level);
        EXPECT_EQ(logger::Logger::getInstance().getLogLevel(), level);
    }
}

/**
 * @test GetFileNameWithUnixPath
 * @brief Verify filename extraction from Unix paths
 */
TEST_F(LoggerTest, GetFileNameWithUnixPath) {
    const char* path = "/home/user/projects/logger/Logger.cpp";
    const char* result = logger::Logger::getFileName(path);

    EXPECT_STREQ(result, "Logger.cpp");
}

/**
 * @test GetFileNameWithWindowsPath
 * @brief Verify filename extraction from Windows paths
 */
TEST_F(LoggerTest, GetFileNameWithWindowsPath) {
    const char* path = "C:\\Users\\user\\projects\\logger\\Logger.cpp";
    const char* result = logger::Logger::getFileName(path);

    EXPECT_STREQ(result, "Logger.cpp");
}

/**
 * @test GetFileNameWithoutPath
 * @brief Verify filename extraction when only filename is provided
 */
TEST_F(LoggerTest, GetFileNameWithoutPath) {
    const char* filename = "Logger.cpp";
    const char* result = logger::Logger::getFileName(filename);

    EXPECT_STREQ(result, "Logger.cpp");
}

/**
 * @test GetFileNameWithNullptr
 * @brief Verify filename extraction handles nullptr gracefully
 */
TEST_F(LoggerTest, GetFileNameWithNullptr) {
    const char* result = logger::Logger::getFileName(nullptr);

    EXPECT_EQ(result, nullptr);
}

/**
 * @test IsCategoryExcludedTrue
 * @brief Verify that excluded categories are recognized
 */
TEST_F(LoggerTest, IsCategoryExcludedTrue) {
    logger::Settings settings;
    settings.excludedCategories = {"server", "network", "database"};

    logger::Logger::getInstance().initialize(settings);

    EXPECT_TRUE(logger::Logger::getInstance().isCategoryExcluded("server"));
    EXPECT_TRUE(logger::Logger::getInstance().isCategoryExcluded("network"));
    EXPECT_TRUE(logger::Logger::getInstance().isCategoryExcluded("database"));
}

/**
 * @test IsCategoryExcludedFalse
 * @brief Verify that non-excluded categories are not recognized as excluded
 */
TEST_F(LoggerTest, IsCategoryExcludedFalse) {
    logger::Settings settings;
    settings.excludedCategories = {"server", "network"};

    logger::Logger::getInstance().initialize(settings);

    EXPECT_FALSE(logger::Logger::getInstance().isCategoryExcluded("client"));
    EXPECT_FALSE(logger::Logger::getInstance().isCategoryExcluded("ui"));
}

/**
 * @test IsCategoryExcludedEmpty
 * @brief Verify no categories are excluded when the list is empty
 */
TEST_F(LoggerTest, IsCategoryExcludedEmpty) {
    logger::Settings settings;
    settings.excludedCategories = {};

    logger::Logger::getInstance().initialize(settings);

    EXPECT_FALSE(logger::Logger::getInstance().isCategoryExcluded("server"));
    EXPECT_FALSE(logger::Logger::getInstance().isCategoryExcluded("client"));
}

/**
 * @test Shutdown
 * @brief Verify logger shutdown
 */
TEST_F(LoggerTest, Shutdown) {
    logger::Logger::setup();

    EXPECT_TRUE(logger::Logger::getInstance().isInitialized());

    logger::Logger::getInstance().shutdown();

    EXPECT_FALSE(logger::Logger::getInstance().isInitialized());
}

/**
 * @test Flush
 * @brief Verify logger flush doesn't throw
 */
TEST_F(LoggerTest, Flush) {
    logger::Logger::setup();

    EXPECT_NO_THROW(logger::Logger::getInstance().flush());
}

/**
 * @test InitializationWithFile
 * @brief Verify logger initialization with file output
 */
TEST_F(LoggerTest, InitializationWithFile) {
    const char* filename = "test.log";

    logger::Settings settings;
    settings.minimumLevel = logger::LogLevel::Info;
    settings.logFileName = filename;

    logger::Logger::getInstance().initialize(settings);

    LOG_INFO("Test log message");
    logger::Logger::getInstance().flush();

    EXPECT_TRUE(fs::exists(filename));
}

/**
 * @test MultipleInitializations
 * @brief Verify logger can be reinitialized
 */
TEST_F(LoggerTest, MultipleInitializations) {
    logger::Logger::setup(logger::LogLevel::Info);
    EXPECT_EQ(logger::Logger::getInstance().getLogLevel(), logger::LogLevel::Info);

    logger::Logger::getInstance().shutdown();

    logger::Logger::setup(logger::LogLevel::Debug);
    EXPECT_EQ(logger::Logger::getInstance().getLogLevel(), logger::LogLevel::Debug);
}

/**
 * @test InitializationSettings
 * @brief Verify all settings are properly applied
 */
TEST_F(LoggerTest, InitializationSettings) {
    logger::Settings settings;
    settings.minimumLevel = logger::LogLevel::Warn;
    settings.showTimestamp = false;
    settings.showThreadInfo = false;
    settings.showSourceLocation = false;
    settings.showFullPath = true;
    settings.excludedCategories = {"internal"};

    logger::Logger::getInstance().initialize(settings);

    EXPECT_TRUE(logger::Logger::getInstance().isInitialized());
    EXPECT_EQ(logger::Logger::getInstance().getLogLevel(), logger::LogLevel::Warn);
}

/**
 * @test LoggingWithoutInitialization
 * @brief Verify logging macros don't crash when logger is not initialized
 */
TEST_F(LoggerTest, LoggingWithoutInitialization) {
    // Logger is not initialized - macros should handle this gracefully
    // Note: We can't use EXPECT_NO_THROW with logging macros directly due to
    // macro expansion issues with braces. Instead we just ensure they compile
    // and don't crash when called uninitialized.
    LOG_INFO("This should not crash");
    LOG_DEBUG("This should not crash");
    LOG_ERROR("This should not crash");
}

/**
 * @test SetupDefaultParameters
 * @brief Verify setup method works with default parameters
 */
TEST_F(LoggerTest, SetupDefaultParameters) {
    logger::Logger::setup();

    EXPECT_TRUE(logger::Logger::getInstance().isInitialized());
}

/**
 * @test SetupWithAllParameters
 * @brief Verify setup method works with all parameters
 */
TEST_F(LoggerTest, SetupWithAllParameters) {
    std::vector<std::string> excluded = {"server", "network"};

    logger::Logger::setup(
        logger::LogLevel::Debug,
        "test.log",
        excluded,
        true,  // showFullPath
        true,  // showTimestamp
        true,  // showThreadInfo
        true   // showSourceLocation
    );

    EXPECT_TRUE(logger::Logger::getInstance().isInitialized());
    EXPECT_EQ(logger::Logger::getInstance().getLogLevel(), logger::LogLevel::Debug);
}

/**
 * @test GetFileNameWithMixedSlashes
 * @brief Verify filename extraction with mixed path separators
 */
TEST_F(LoggerTest, GetFileNameWithMixedSlashes) {
    // Test with trailing slash
    const char* path1 = "/home/user/project/";
    const char* result1 = logger::Logger::getFileName(path1);
    EXPECT_STREQ(result1, "");

    // Test single slash
    const char* path2 = "/file.cpp";
    const char* result2 = logger::Logger::getFileName(path2);
    EXPECT_STREQ(result2, "file.cpp");
}
