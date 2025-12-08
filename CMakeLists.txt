cmake_minimum_required(VERSION 3.21)
project(R-Type VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to enable code coverage
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0)
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

# Make asio and spdlog available globally for all subdirectories
set(COMMON_LIBS asio spdlog::spdlog)

# Add subdirectories for libraries and executables
add_subdirectory(src/engine)
add_subdirectory(src/common)
add_subdirectory(src/server)
add_subdirectory(src/client)

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)