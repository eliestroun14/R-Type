cmake_minimum_required(VERSION 3.21)
project(R-Type VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Add logger subdirectory
add_subdirectory(src/logger)

set(SOURCES
    "src/main.cpp"
)

add_executable(R-Type ${SOURCES})

# Link logger library
target_link_libraries(R-Type PRIVATE logger)