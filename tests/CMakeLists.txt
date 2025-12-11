# Include your source directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Create test executable for ECS
add_executable(ecs_tests
   # Root tests
   TestDebug.cpp
   Error_messages_test.cpp
   
   # Common tests
   common/TestPacket.cpp
   
   # Engine tests
   engine/LoggerTest.cpp
   
   # Coordinator network tests
   engine/coordinator/network/TestNetworkManagerCreate.cpp
   engine/coordinator/network/TestNetworkManagers.cpp
   engine/coordinator/network/TestPacketManagerAssert.cpp
   engine/coordinator/network/TestProcessPacket.cpp
   
   # Game engine coordinator tests
   engine/gameEngine/coordinator/TestCoordinator.cpp
   
   # ECS component tests
   engine/gameEngine/coordinator/ecs/component/TestComponentManager.cpp
   
   # ECS entity tests
   engine/gameEngine/coordinator/ecs/entity/TestEntity.cpp
   engine/gameEngine/coordinator/ecs/entity/TestEntityManager.cpp
   
   # ECS system tests
   engine/gameEngine/coordinator/ecs/system/TestSystem.cpp
   engine/gameEngine/coordinator/ecs/system/TestSystemManager.cpp
   
   # Render tests
   engine/gameEngine/coordinator/render/TestRenderManager.cpp
)

# Link Google Test and your libraries
target_link_libraries(ecs_tests
    gtest
    gtest_main
    engine
    common
    client_lib
    server_lib
    spdlog::spdlog
    sfml-graphics
    sfml-window
    sfml-system
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(ecs_tests)