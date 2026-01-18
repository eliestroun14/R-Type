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
    game
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
    game
    ${COMMON_LIBS}
)

# Link rendering/audio libraries (SFML)
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
target_link_libraries(r-type-client PRIVATE sfml-graphics sfml-window sfml-system)

# Locate OpenAL32.dll shipped with SFML (several common paths)
set(OPENAL_DLL_PATH "")
if(EXISTS "${SFML_SOURCE_DIR}/extlibs/bin/x64/OpenAL32.dll")
    set(OPENAL_DLL_PATH "${SFML_SOURCE_DIR}/extlibs/bin/x64/OpenAL32.dll")
elseif(EXISTS "${SFML_SOURCE_DIR}/extlibs/bin/x86/OpenAL32.dll")
    set(OPENAL_DLL_PATH "${SFML_SOURCE_DIR}/extlibs/bin/x86/OpenAL32.dll")
elseif(EXISTS "${CMAKE_BINARY_DIR}/_deps/sfml-build/extlibs/bin/x64/OpenAL32.dll")
    set(OPENAL_DLL_PATH "${CMAKE_BINARY_DIR}/_deps/sfml-build/extlibs/bin/x64/OpenAL32.dll")
elseif(EXISTS "${CMAKE_BINARY_DIR}/_deps/sfml-build/extlibs/bin/x86/OpenAL32.dll")
    set(OPENAL_DLL_PATH "${CMAKE_BINARY_DIR}/_deps/sfml-build/extlibs/bin/x86/OpenAL32.dll")
endif()

add_custom_command(TARGET r-type-client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:r-type-client>/assets
    COMMENT "Copying assets..."
)

# Copy dependent DLLs (SFML, etc.) next to the client executable after each build
# This is only needed on Windows
if(WIN32)
    add_custom_command(TARGET r-type-client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:r-type-client>
            $<TARGET_FILE_DIR:r-type-client>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLLs for r-type-client..."
    )
endif()

# Copy OpenAL32.dll if found so the executable can run immediately
# This is only needed on Windows
if(WIN32 AND OPENAL_DLL_PATH)
    add_custom_command(TARGET r-type-client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OPENAL_DLL_PATH}"
            $<TARGET_FILE_DIR:r-type-client>
        COMMENT "Copying OpenAL32.dll for r-type-client..."
    )
endif()
