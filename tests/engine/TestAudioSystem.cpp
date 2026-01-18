#include <gtest/gtest.h>

#include <game/systems/AudioSystem.hpp>

TEST(AudioSystemCoverage, ConstructorAndOnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    AudioSystem system(engine);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}
