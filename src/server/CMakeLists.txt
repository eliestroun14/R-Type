# Server Application CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(r-type-server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Server sources
file(GLOB_RECURSE SERVER_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Server headers
file(GLOB_RECURSE SERVER_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# Create server library
add_library(server_lib STATIC ${SERVER_SOURCES} ${SERVER_HEADERS})

target_include_directories(server_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Ensure server can include game public headers
target_include_directories(server_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src/game/include
)
# Link libraries
target_link_libraries(server_lib PUBLIC
    engine
    common
    game
    ${COMMON_LIBS}
)

# Link platform-specific network libraries
if(WIN32)
    target_link_libraries(server_lib PUBLIC ws2_32 wsock32)
endif()

# Create server executable
add_executable(r-type-server ${SERVER_SOURCES} ${SERVER_HEADERS})

target_include_directories(r-type-server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/game/include
)

# Link libraries
target_link_libraries(r-type-server PRIVATE 
    engine
    common
    game
    ${COMMON_LIBS}
)
