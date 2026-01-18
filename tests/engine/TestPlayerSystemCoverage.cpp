#include <gtest/gtest.h>

#include <engine/GameEngine.hpp>
#include <engine/ecs/component/Components.hpp>
#include <common/constants/defines.hpp>
#include <common/error/Error.hpp>
#include <game/systems/PlayerSystem.hpp>

namespace {

class TestablePlayerSystem : public PlayerSystem {
public:
    using PlayerSystem::PlayerSystem;

    void addEntity(std::size_t id)
    {
        _entities.push_back(id);
    }
};

PlayerSystem& setupPlayerSystem(gameEngine::GameEngine& engine)
{
    engine.init();
    engine.registerComponent<Velocity>();
    engine.registerComponent<InputComponent>();
    engine.registerComponent<Animation>();

    auto& system = engine.registerSystem<PlayerSystem>(engine);
    engine.setSystemSignature<PlayerSystem, Velocity, InputComponent>();
    return system;
}

Entity createPlayerEntity(gameEngine::GameEngine& engine)
{
    Entity entity = engine.createEntity("player");
    engine.addComponent(entity, Velocity(0.0f, 0.0f));
    engine.addComponent(entity, InputComponent(1));
    engine.addComponent(entity, Animation(32, 32, 0, 0.0f, 0.1f, 2, 2, true));
    return entity;
}

} // namespace

TEST(PlayerSystemCoverage, OnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    PlayerSystem system(engine);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}

TEST(PlayerSystemCoverage, ThrowsWhenComponentsNotRegistered)
{
    gameEngine::GameEngine engine;
    engine.init();

    PlayerSystem system(engine);
    EXPECT_THROW(system.onUpdate(0.1f), Error);
}
