#include <gtest/gtest.h>

#include <engine/GameEngine.hpp>
#include <engine/ecs/component/Components.hpp>
#include <game/systems/LevelTimerSystem.hpp>

namespace {

LevelTimerSystem& setupTimerSystem(gameEngine::GameEngine& engine)
{
    engine.init();
    engine.registerComponent<Level>();
    engine.registerComponent<Text>();
    engine.registerComponent<TimerUI>();

    auto& system = engine.registerSystem<LevelTimerSystem>(engine);
    engine.setSystemSignature<LevelTimerSystem, TimerUI, Text>();
    return system;
}

Entity createTimerEntity(gameEngine::GameEngine& engine, const char* label)
{
    Entity entity = engine.createEntity("timer");
    engine.addComponent(entity, TimerUI{});
    engine.addComponent(entity, Text(label, sf::Color::White, 14, ZIndex::IS_UI_HUD));
    return entity;
}

Entity createLevelEntity(gameEngine::GameEngine& engine, bool started, bool completed, float duration, float elapsed)
{
    Entity entity = engine.createEntity("level");
    Level level;
    level.started = started;
    level.completed = completed;
    level.levelDuration = duration;
    level.elapsedTime = elapsed;
    engine.addComponent(entity, level);
    return entity;
}

} // namespace

TEST(LevelTimerSystemCoverage, ConstructorAndOnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    LevelTimerSystem system(engine);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}

TEST(LevelTimerSystemCoverage, SkipsWhenNoActiveLevel)
{
    gameEngine::GameEngine engine;
    auto& system = setupTimerSystem(engine);

    Entity timer = createTimerEntity(engine, "init");
    createLevelEntity(engine, false, false, 10.0f, 0.0f);

    system.onUpdate(0.1f);

    auto& text = engine.getComponents<Text>()[static_cast<size_t>(timer)].value();
    EXPECT_STREQ(text.str, "init");
    EXPECT_EQ(text.color, sf::Color::White);
}

TEST(LevelTimerSystemCoverage, SkipsEntitiesWithoutTextComponent)
{
    gameEngine::GameEngine engine;
    auto& system = setupTimerSystem(engine);

    Entity timer = createTimerEntity(engine, "before");
    createLevelEntity(engine, true, false, 10.0f, 1.0f);

    auto& texts = engine.getComponents<Text>();
    texts[static_cast<size_t>(timer)].reset();

    system.onUpdate(0.1f);

    EXPECT_FALSE(texts[static_cast<size_t>(timer)].has_value());
}
