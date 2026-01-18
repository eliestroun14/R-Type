cmake_minimum_required(VERSION 3.21)
project(R-Type VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to enable code coverage
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    endif()
endif()

# Include CPM (C++ Package Manager)
include(cmake/CPM.cmake)

# Add spdlog dependency (with fmt bundled)
CPMAddPackage(
    NAME spdlog
    VERSION 1.12.0
    GITHUB_REPOSITORY gabime/spdlog
    OPTIONS
        "SPDLOG_FMT_EXTERNAL OFF"
)

# Add Asio dependency (header-only library for networking)
CPMAddPackage(
    NAME asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
    SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/asio-src
    DOWNLOAD_ONLY YES
)

if(asio_ADDED)
    add_library(asio INTERFACE IMPORTED)
    target_include_directories(asio INTERFACE ${CMAKE_BINARY_DIR}/_deps/asio-src/asio/include)
endif()

# Add Google Test dependency (for tests)
CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    VERSION 1.14.0
    OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt ON"
)

# Add SFML dependency (for render, graphical lib) -> download/compile it
CPMAddPackage(
    NAME SFML
    GITHUB_REPOSITORY SFML/SFML
    GIT_TAG 2.6.1
    CMAKE_ARGS
        "-DBUILD_SHARED_LIBS=OFF"
        "-DSFML_BUILD_EXAMPLES=OFF"
        "-DSFML_BUILD_DOC=OFF"
        "-DSFML_BUILD_TEST_SUITE=OFF"
        "-DSFML_USE_SYSTEM_DEPS=OFF"
        "-DSFML_ENABLE_FLAC=OFF"
)

# SFML targets are: sfml-system, sfml-window, sfml-graphics, sfml-audio, sfml-network
message(STATUS "SFML_ADDED: ${SFML_ADDED}")

# Make asio and spdlog available globally for all subdirectories
set(COMMON_LIBS asio spdlog::spdlog)

# Add subdirectories for libraries and executables
add_subdirectory(src/common)
add_subdirectory(src/engine)
add_subdirectory(src/server)
add_subdirectory(src/client)
add_subdirectory(src/game)

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)

# coverage
if(ENABLE_COVERAGE)
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)

    if(LCOV AND GENHTML)
        set(COVERAGE_EXCLUDES
            '/usr/*'
            '${CMAKE_SOURCE_DIR}/tests/*'
            '${CMAKE_BINARY_DIR}/_deps/*'
        )

        set(EXCLUDE_OPTIONS "")
        foreach(PATTERN ${COVERAGE_EXCLUDES})
            list(APPEND EXCLUDE_OPTIONS "--exclude" "${PATTERN}")
        endforeach()

        add_custom_target(coverage
            COMMAND ${LCOV} --directory . --capture --output-file coverage.info ${EXCLUDE_OPTIONS} --ignore-errors unused --ignore-errors mismatch
            COMMAND ${GENHTML} coverage.info --output-directory coverage_report --ignore-errors unused
            COMMAND echo "Coverage report generated in: coverage_report/index.html"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report..."
        )

        add_custom_target(coverage_clean
            COMMAND ${LCOV} --directory . --zerocounters
            COMMAND find . -name '*.gcda' -delete
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Cleaning coverage data..."
        )
    endif()
endif()