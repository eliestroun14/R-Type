#include <gtest/gtest.h>

#include <game/systems/AnimationSystem.hpp>

TEST(AnimationSystemCoverage, ConstructorAndOnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    AnimationSystem system(engine);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}
