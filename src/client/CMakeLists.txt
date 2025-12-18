# Client Application CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(r-type-client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Client sources (exclude main.cpp from library)
file(GLOB_RECURSE CLIENT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
list(REMOVE_ITEM CLIENT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Client headers
file(GLOB_RECURSE CLIENT_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# Create client library (without main.cpp)
add_library(client_lib STATIC ${CLIENT_SOURCES} ${CLIENT_HEADERS})

target_include_directories(client_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/game/include
)

# Link libraries
target_link_libraries(client_lib PUBLIC
    engine
    common
    ${COMMON_LIBS}
)

# Create client executable (with main.cpp)
add_executable(r-type-client 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_include_directories(r-type-client PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(r-type-client PRIVATE
    client_lib
    engine
    common
    ${COMMON_LIBS}
)

# Link rendering/audio libraries (SFML)
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
target_link_libraries(r-type-client PRIVATE sfml-graphics sfml-window sfml-system)

add_custom_command(TARGET r-type-client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:r-type-client>/assets
    COMMENT "Copying assets..."
)
