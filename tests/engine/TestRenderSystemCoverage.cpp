#include <gtest/gtest.h>

#define private public
#include <engine/GameEngine.hpp>
#undef private

#include <engine/ecs/component/Components.hpp>
#include <common/constants/render/Assets.hpp>
#include <game/systems/RenderSystem.hpp>
#include <filesystem>

namespace {

RenderSystem& setupRenderSystem(gameEngine::GameEngine& engine)
{
    engine.init();
    engine.registerComponent<Transform>();
    engine.registerComponent<Sprite>();
    engine.registerComponent<Text>();
    engine.registerComponent<ScrollingBackground>();
    engine.registerComponent<GameConfig>();

    auto& system = engine.registerSystem<RenderSystem>(engine);
    engine.setSystemSignature<RenderSystem, Transform, Sprite>();
    return system;
}

Entity createSpriteEntity(gameEngine::GameEngine& engine,
                          const char* name,
                          Assets asset,
                          ZIndex z,
                          const sf::IntRect& rect,
                          const Transform& transform)
{
    Entity entity = engine.createEntity(name);
    engine.addComponent(entity, Transform(transform));
    engine.addComponent(entity, Sprite(asset, z, rect));
    return entity;
}

Entity createTextEntity(gameEngine::GameEngine& engine,
                        const char* name,
                        const char* content,
                        unsigned int size,
                        ZIndex z,
                        const Transform& transform)
{
    Entity entity = engine.createEntity(name);
    engine.addComponent(entity, Transform(transform));
    engine.addComponent(entity, Text(content, sf::Color::White, size, z));
    return entity;
}

} // namespace

TEST(RenderSystemCoverage, ConstructorAndOnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    RenderSystem system(engine);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}

TEST(RenderSystemCoverage, HandlesTextOnlyAndNullAssets)
{
    gameEngine::GameEngine engine;
    auto& system = setupRenderSystem(engine);

    engine._renderManager = std::make_shared<RenderManager>();

    Entity sprite = createSpriteEntity(
        engine,
        "sprite",
        Assets::LOGO_RTYPE,
        ZIndex::IS_GAME,
        sf::IntRect(0, 0, 0, 0),
        Transform(10.0f, 20.0f, 0.0f, 1.0f));

    Entity textEntity = createTextEntity(
        engine,
        "text",
        "warn",
        0,
        ZIndex::IS_UI_HUD,
        Transform(50.0f, 60.0f, 0.0f, 1.0f));

    Entity configEntity = engine.createEntity("config");
    engine.addComponent(configEntity, GameConfig(FontAssets::DEFAULT_FONT, true, true));

    system.onUpdate(0.1f);

    EXPECT_TRUE(engine.getComponents<Text>()[static_cast<size_t>(textEntity)].has_value());
    EXPECT_TRUE(engine.getComponents<Sprite>()[static_cast<size_t>(sprite)].has_value());
}
