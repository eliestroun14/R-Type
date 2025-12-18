# Game Library CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(r-type-game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Game sources
file(GLOB_RECURSE GAME_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Game headers
file(GLOB_RECURSE GAME_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# Create game library
add_library(game STATIC ${GAME_SOURCES} ${GAME_HEADERS})

target_include_directories(game PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(game PUBLIC
    engine
    common
    ${COMMON_LIBS}
)
