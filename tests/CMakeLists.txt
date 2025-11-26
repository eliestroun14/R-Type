# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Include your source directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/gameEngine)
include_directories(${CMAKE_SOURCE_DIR}/src/gameEngine/ECS)

# Create test executable for ECS
add_executable(ecs_tests
    test_entity.cpp
    test_component_manager.cpp
    test_entity_manager.cpp
    test_game_engine.cpp
    test_debug.cpp
    test_system_manager.cpp

    ${CMAKE_SOURCE_DIR}/src/gameEngine/ECS/System/System.cpp
)

# Link Google Test and your libraries
target_link_libraries(ecs_tests
    GTest::GTest
    GTest::Main
    pthread
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(ecs_tests)
