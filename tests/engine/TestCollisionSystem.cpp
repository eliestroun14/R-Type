#include <gtest/gtest.h>

#include <game/systems/CollisionSystem.hpp>

TEST(CollisionSystemCoverage, ConstructorAndOnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    CollisionSystem system(engine);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}
