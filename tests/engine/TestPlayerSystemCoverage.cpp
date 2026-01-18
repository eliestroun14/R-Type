#include <gtest/gtest.h>

#include <game/systems/PlayerSystem.hpp>

TEST(PlayerSystemCoverage, OnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    PlayerSystem system(engine);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}
