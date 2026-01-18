#include <gtest/gtest.h>

#include <game/systems/RenderSystem.hpp>

TEST(RenderSystemCoverage, ConstructorAndOnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    RenderSystem system(engine);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}
