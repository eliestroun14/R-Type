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

# Server sources
set(SERVER_SOURCES
    "src/main.cpp"
    "src/server/Server.cpp"
    "src/server/network/Network.cpp"
    "src/server/session/Session.cpp"
)

# Create main server executable
add_executable(r-type_server ${SERVER_SOURCES})

# Include directories for the server
target_include_directories(r-type_server PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/gameEngine
    ${CMAKE_SOURCE_DIR}/src/gameEngine/ECS
)

# Link logger library
target_link_libraries(r-type_server PRIVATE logger)

# Platform-specific network libraries
if(WIN32)
    target_link_libraries(r-type_server PRIVATE ws2_32)
endif()

# Test client for debugging
set(TEST_CLIENT_SOURCES
    "src/server/utils/test_client.cpp"
)

add_executable(test_client ${TEST_CLIENT_SOURCES})

target_include_directories(test_client PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

if(WIN32)
    target_link_libraries(test_client PRIVATE ws2_32)
endif()

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)