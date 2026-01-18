#include <gtest/gtest.h>

#include <game/systems/BackgroundSystem.hpp>

TEST(BackgroundSystemCoverage, ConstructorAndOnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    BackgroundSystem system(engine, false);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}

TEST(BackgroundSystemCoverage, ConstructorWithServerFlag)
{
    gameEngine::GameEngine engine;
    engine.init();

    BackgroundSystem system(engine, true);
    system.onCreate();
    SUCCEED();
}
