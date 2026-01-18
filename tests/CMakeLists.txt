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

   game/TestGameInlineFunctions.cpp

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

# Create separate test executable for pure C++ tests without SFML dependency
add_executable(pure_cpp_tests
    game/TestIMenu.cpp
    game/TestGame.cpp
)

target_include_directories(pure_cpp_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/game/include
    ${CMAKE_SOURCE_DIR}/src/common/include
)

# Link only essential libraries (no SFML/graphics)
target_link_libraries(pure_cpp_tests
    GTest::gtest
    GTest::gtest_main
)

gtest_discover_tests(pure_cpp_tests)

# Game.cpp coverage tests compiled with stub headers (no SFML dependency)
add_executable(game_coverage_tests
    game/TestGameCoverage.cpp
    ${CMAKE_SOURCE_DIR}/src/game/src/Game.cpp
)

# Stubs MUST come first to override real headers
target_include_directories(game_coverage_tests PRIVATE BEFORE
    ${CMAKE_SOURCE_DIR}/tests/stubs
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/game/include
    ${CMAKE_SOURCE_DIR}/src/common/include
    ${CMAKE_SOURCE_DIR}/src/engine/include
)

target_link_libraries(game_coverage_tests
    GTest::gtest
    GTest::gtest_main
)

# Add coverage flags for coverage analysis
target_compile_options(game_coverage_tests PRIVATE --coverage)
target_link_options(game_coverage_tests PRIVATE --coverage)

gtest_discover_tests(game_coverage_tests)

# ClientUtils.cpp coverage tests compiled with stub headers (no SFML dependency)
add_executable(client_utils_coverage_tests
    game/TestClientUtilsCoverage.cpp
    ${CMAKE_SOURCE_DIR}/src/game/src/utils/ClientUtils.cpp
)

target_include_directories(client_utils_coverage_tests PRIVATE BEFORE
    ${CMAKE_SOURCE_DIR}/tests/stubs
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/game/include
    ${CMAKE_SOURCE_DIR}/src/common/include
    ${CMAKE_SOURCE_DIR}/src/engine/include
)

target_link_libraries(client_utils_coverage_tests
    GTest::gtest
    GTest::gtest_main
)

target_compile_options(client_utils_coverage_tests PRIVATE --coverage)
target_link_options(client_utils_coverage_tests PRIVATE --coverage)

gtest_discover_tests(client_utils_coverage_tests)

# AnimationSystem.cpp coverage tests compiled with stub headers (no SFML dependency)
add_executable(animation_system_coverage_tests
    game/TestAnimationSystemCoverage.cpp
    ${CMAKE_SOURCE_DIR}/src/game/src/systems/AnimationSystem.cpp
)

target_include_directories(animation_system_coverage_tests PRIVATE BEFORE
    ${CMAKE_SOURCE_DIR}/tests/stubs
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/game/include
    ${CMAKE_SOURCE_DIR}/src/common/include
    ${CMAKE_SOURCE_DIR}/src/engine/include
)

target_link_libraries(animation_system_coverage_tests
    GTest::gtest
    GTest::gtest_main
)

target_compile_options(animation_system_coverage_tests PRIVATE --coverage)
target_link_options(animation_system_coverage_tests PRIVATE --coverage)

gtest_discover_tests(animation_system_coverage_tests)

# AccessibilitySystem.cpp coverage tests compiled with stub headers (no SFML dependency)
add_executable(accessibility_system_coverage_tests
    game/TestAccessibilitySystemCoverage.cpp
    ${CMAKE_SOURCE_DIR}/src/game/src/systems/AccessibilitySystem.cpp
)

target_include_directories(accessibility_system_coverage_tests PRIVATE BEFORE
    ${CMAKE_SOURCE_DIR}/tests/stubs
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/game/include
    ${CMAKE_SOURCE_DIR}/src/common/include
    ${CMAKE_SOURCE_DIR}/src/engine/include
)

target_link_libraries(accessibility_system_coverage_tests
    GTest::gtest
    GTest::gtest_main
)

target_compile_options(accessibility_system_coverage_tests PRIVATE --coverage)
target_link_options(accessibility_system_coverage_tests PRIVATE --coverage)

gtest_discover_tests(accessibility_system_coverage_tests)

# AudioSystem.cpp coverage tests compiled with stub headers (no SFML dependency)
add_executable(audio_system_coverage_tests
    game/TestAudioSystemCoverage.cpp
    ${CMAKE_SOURCE_DIR}/src/game/src/systems/AudioSystem.cpp
)

target_include_directories(audio_system_coverage_tests PRIVATE BEFORE
    ${CMAKE_SOURCE_DIR}/tests/stubs
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/game/include
    ${CMAKE_SOURCE_DIR}/src/common/include
    ${CMAKE_SOURCE_DIR}/src/engine/include
)

target_link_libraries(audio_system_coverage_tests
    GTest::gtest
    GTest::gtest_main
)

target_compile_options(audio_system_coverage_tests PRIVATE --coverage)
target_link_options(audio_system_coverage_tests PRIVATE --coverage)

gtest_discover_tests(audio_system_coverage_tests)

# BackgroundSystem.cpp coverage tests compiled with stub headers (no SFML dependency)
add_executable(background_system_coverage_tests
    game/TestBackgroundSystemCoverage.cpp
    ${CMAKE_SOURCE_DIR}/src/game/src/systems/BackgroundSystem.cpp
)

target_include_directories(background_system_coverage_tests PRIVATE BEFORE
    ${CMAKE_SOURCE_DIR}/tests/stubs
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/game/include
    ${CMAKE_SOURCE_DIR}/src/common/include
    ${CMAKE_SOURCE_DIR}/src/engine/include
)

target_link_libraries(background_system_coverage_tests
    GTest::gtest
    GTest::gtest_main
)

target_compile_options(background_system_coverage_tests PRIVATE --coverage)
target_link_options(background_system_coverage_tests PRIVATE --coverage)

gtest_discover_tests(background_system_coverage_tests)

# ButtonSystem.cpp coverage tests compiled with stub headers (no SFML dependency)
add_executable(button_system_coverage_tests
    game/TestButtonSystemCoverage.cpp
    ${CMAKE_SOURCE_DIR}/src/game/src/systems/ButtonSystem.cpp
)

target_include_directories(button_system_coverage_tests PRIVATE BEFORE
    ${CMAKE_SOURCE_DIR}/tests/stubs
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/game/include
    ${CMAKE_SOURCE_DIR}/src/common/include
    ${CMAKE_SOURCE_DIR}/src/engine/include
)

target_link_libraries(button_system_coverage_tests
    GTest::gtest
    GTest::gtest_main
)

target_compile_options(button_system_coverage_tests PRIVATE --coverage)
target_link_options(button_system_coverage_tests PRIVATE --coverage)

gtest_discover_tests(button_system_coverage_tests)

# Client menu coverage tests (uses ClientMenu.cpp with test stubs for ClientUtils)
add_executable(client_menu_tests
    client/TestClientMenu.cpp
    client/ClientUtilsTestStubs.cpp
    ${CMAKE_SOURCE_DIR}/src/client/src/menu/ClientMenu.cpp
)

target_include_directories(client_menu_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/client/include
    ${CMAKE_SOURCE_DIR}/src/game/include
    ${CMAKE_SOURCE_DIR}/src/common/include
    ${CMAKE_SOURCE_DIR}/src/engine/include
)

target_link_libraries(client_menu_tests
    GTest::gtest
    GTest::gtest_main
    engine
    common
    sfml-graphics
    sfml-window
    sfml-system
)

gtest_discover_tests(client_menu_tests)
