#include <gtest/gtest.h>

#include <game/systems/LevelTimerSystem.hpp>

TEST(LevelTimerSystemCoverage, ConstructorAndOnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    LevelTimerSystem system(engine);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}
