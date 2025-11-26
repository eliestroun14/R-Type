cmake_minimum_required(VERSION 3.10.0)
project(R-Type VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files for main executable
set(SOURCES
    "src/main.cpp"
)

# Create main executable
add_executable(R-Type ${SOURCES})

# Include directories for the main target
target_include_directories(R-Type PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/gameEngine
    ${CMAKE_SOURCE_DIR}/src/gameEngine/ECS
)

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)