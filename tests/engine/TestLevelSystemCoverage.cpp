#include <gtest/gtest.h>

#include <game/systems/LevelSystem.hpp>

TEST(LevelSystemCoverage, OnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    LevelSystem system(engine, nullptr);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}
