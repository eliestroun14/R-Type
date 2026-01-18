#include <gtest/gtest.h>

#include <engine/GameEngine.hpp>
#include <common/constants/render/Assets.hpp>
#include <game/systems/CollisionSystem.hpp>
#include <game/systems/ScoreSystem.hpp>

namespace {

CollisionSystem& setupCollisionSystem(gameEngine::GameEngine& engine)
{
    engine.init();

    engine.registerComponent<Transform>();
    engine.registerComponent<Sprite>();
    engine.registerComponent<Health>();
    engine.registerComponent<HitBox>();
    engine.registerComponent<Projectile>();
    engine.registerComponent<Team>();

    engine.registerSystem<ScoreSystem>(engine);
    auto& collisionSystem = engine.registerSystem<CollisionSystem>(engine);
    engine.setSystemSignature<CollisionSystem, Transform, Sprite, HitBox>();

    return collisionSystem;
}

Entity createCollidable(gameEngine::GameEngine& engine,
                        float x,
                        float y,
                        int width,
                        int height)
{
    Entity entity = engine.createEntity("collidable");
    engine.addComponent(entity, Transform(x, y, 0.0f, 1.0f));
    engine.addComponent(entity, Sprite(Assets::LOGO_RTYPE, ZIndex::IS_GAME, sf::IntRect(0, 0, width, height)));
    engine.addComponent(entity, HitBox{});
    return entity;
}

} // namespace

TEST(CollisionSystemCoverage, ConstructorAndOnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    CollisionSystem system(engine);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}