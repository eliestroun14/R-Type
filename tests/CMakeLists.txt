# Include your source directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Create test executable for ECS
add_executable(ecs_tests

   engine/gameEngine/coordinator/ecs/entity/TestEntity.cpp
   engine/gameEngine/coordinator/ecs/entity/TestEntityManager.cpp
   engine/gameEngine/coordinator/ecs/component/TestComponentManager.cpp
   engine/gameEngine/coordinator/ecs/system/TestSystemManager.cpp
    engine/gameEngine/coordinator/ecs/system/TestSystem.cpp

   engine/gameEngine/coordinator/render/TestRenderManager.cpp

    engine/TestRenderManagerCoverage.cpp

   engine/gameEngine/coordinator/TestCoordinator.cpp

   engine/LoggerTest.cpp
    engine/audio/TestAudioManager.cpp
    engine/audio/TestAudioStorage.cpp
   engine/coordinator/network/TestPacketManagerAssert.cpp
   engine/coordinator/network/TestProcessPacket.cpp
   engine/coordinator/network/TestNetworkManagerCreate.cpp
    engine/coordinator/network/TestPacketManagerCreateCoverage.cpp
   engine/coordinator/network/TestNetworkManagers.cpp
   common/TestPacket.cpp
    common/TestASocket.cpp
    common/TestAsioSocket.cpp

    engine/TestGameEngine.cpp
    engine/TestComponents.cpp
    engine/TestEntityManagerCoverage.cpp
    engine/TestFontStorage.cpp
    engine/TestTextureStorage.cpp
    engine/TestAnimationSystem.cpp
    engine/TestAudioSystem.cpp
    engine/TestBackgroundSystem.cpp
    engine/TestCollisionSystem.cpp
    engine/TestLevelTimerSystem.cpp
    engine/TestRenderSystemCoverage.cpp
    engine/TestPlayerSystemCoverage.cpp
    engine/TestLevelSystemCoverage.cpp

   client/TestRTypeClient.cpp
   client/TestClientNetworkManager.cpp
   client/TestClientNetworkManagerImpl.cpp
   client/TestRTypeClientImpl.cpp

   TestDebug.cpp
    Error_messages_test.cpp
)

# Link Google Test and your libraries
target_link_libraries(ecs_tests
    GTest::gtest
    GTest::gtest_main
    engine
    common
    client_lib
    server_lib
    game
    spdlog::spdlog

    sfml-graphics
    sfml-window
    sfml-system
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(ecs_tests)

# Game tests removed
