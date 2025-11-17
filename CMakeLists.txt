cmake_minimum_required(VERSION 3.10.0)
project(R-Type VERSION 0.1.0 LANGUAGES C CXX)

set(SOURCES
    "src/main.cpp"
)

add_executable(R-Type ${SOURCES})