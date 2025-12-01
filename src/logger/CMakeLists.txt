cmake_minimum_required(VERSION 3.15)

add_library(logger STATIC)

target_sources(logger PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logger.cpp
)

target_compile_features(logger PUBLIC cxx_std_17)

target_include_directories(logger
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(logger PUBLIC spdlog::spdlog)

if(MSVC)
    target_compile_options(logger PRIVATE /W4)
else()
    target_compile_options(logger PRIVATE -Wall -Wextra -Wpedantic)
endif()
