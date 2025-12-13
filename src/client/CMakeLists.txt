# Client Application CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(r-type-client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Client sources
file(GLOB_RECURSE CLIENT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Client headers
file(GLOB_RECURSE CLIENT_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# Create client library
add_library(client_lib STATIC ${CLIENT_SOURCES} ${CLIENT_HEADERS})

target_include_directories(client_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(client_lib PUBLIC
    engine
    common
    ${COMMON_LIBS}
)

# Create client executable
add_executable(r-type-client ${CLIENT_SOURCES} ${CLIENT_HEADERS})

target_include_directories(r-type-client PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(r-type-client PRIVATE 
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
