# Common Library CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(r-type-common)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common sources
file(GLOB_RECURSE COMMON_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Common headers
file(GLOB_RECURSE COMMON_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# Create common library
add_library(common STATIC ${COMMON_SOURCES} ${COMMON_HEADERS})

target_include_directories(common PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/engine/include
)

# Link common dependencies
target_link_libraries(common PUBLIC ${COMMON_LIBS} sfml-graphics sfml-window sfml-system)

# Link platform-specific network libraries
if(WIN32)
    target_link_libraries(common PUBLIC ws2_32 wsock32)
endif()
