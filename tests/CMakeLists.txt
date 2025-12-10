# Include your source directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Create test executable for ECS
add_executable(ecs_tests

   engine/gameEngine/coordinator/ecs/entity/TestEntity.cpp
   engine/gameEngine/coordinator/ecs/entity/TestEntityManager.cpp
   engine/gameEngine/coordinator/ecs/component/TestComponentManager.cpp
   engine/gameEngine/coordinator/ecs/system/TestSystemManager.cpp

   engine/gameEngine/coordinator/render/TestRenderManager.cpp

   engine/gameEngine/coordinator/TestCoordinator.cpp

   engine/LoggerTest.cpp
   engine/coordinator/network/TestNetworkManagerAssert.cpp
   engine/coordinator/network/TestProcessPacket.cpp
   common/TestPacket.cpp

   TestDebug.cpp
    Error_messages_test.cpp
)

# Link Google Test and your libraries
target_link_libraries(ecs_tests
    GTest::gtest
    GTest::gtest_main
    pthread
    engine
    common
    spdlog::spdlog

    sfml-graphics
    sfml-window
    sfml-system
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(ecs_tests)
