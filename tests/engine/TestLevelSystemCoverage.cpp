#include <gtest/gtest.h>

#include <common/constants/defines.hpp>
#include <engine/ecs/component/Components.hpp>
#include <game/coordinator/Coordinator.hpp>
#include <game/systems/LevelSystem.hpp>

namespace {

Level makeLevel(bool started, float duration)
{
    Level level;
    level.started = started;
    level.levelDuration = duration;
    level.completed = false;
    level.elapsedTime = 0.0f;
    level.currentWaveIndex = 0;
    return level;
}

Entity createLevelEntity(gameEngine::GameEngine& engine, Level level)
{
    Entity entity = engine.createEntity("level");
    engine.addComponent(entity, std::move(level));
    return entity;
}

} // namespace

TEST(LevelSystemCoverage, OnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    LevelSystem system(engine, nullptr);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}

TEST(LevelSystemCoverage, SkipsWhenNotStartedOrCompleted)
{
    Coordinator coord;
    coord.initEngine();

    auto engine = coord.getEngine();
    auto& system = engine->getSystem<LevelSystem>();

    Level level = makeLevel(false, 0.0f);
    Entity entity = createLevelEntity(*engine, level);

    system.onUpdate(1.0f);

    auto& updated = engine->getComponents<Level>()[static_cast<size_t>(entity)].value();
    EXPECT_FLOAT_EQ(updated.elapsedTime, 0.0f);
    EXPECT_FALSE(updated.completed);

    updated.started = true;
    updated.completed = true;
    system.onUpdate(1.0f);

    EXPECT_TRUE(updated.completed);
    EXPECT_FLOAT_EQ(updated.elapsedTime, 0.0f);
}

TEST(LevelSystemCoverage, CompletesOnDurationLimit)
{
    Coordinator coord;
    coord.initEngine();

    auto engine = coord.getEngine();
    auto& system = engine->getSystem<LevelSystem>();

    Level level = makeLevel(true, 1.0f);
    Entity entity = createLevelEntity(*engine, level);

    system.onUpdate(1.5f);

    auto& updated = engine->getComponents<Level>()[static_cast<size_t>(entity)].value();
    EXPECT_TRUE(updated.completed);
    EXPECT_FLOAT_EQ(updated.elapsedTime, 1.5f);
}

TEST(LevelSystemCoverage, SpawnsEnemiesAndCompletesOnWaves)
{
    Coordinator coord;
    coord.initEngine();

    auto engine = coord.getEngine();
    auto& system = engine->getSystem<LevelSystem>();

    Level level = makeLevel(true, 0.0f);
    Wave wave;
    wave.startTime = 0.0f;
    wave.enemies = {
        {EnemyType::BASIC, 10.0f, 20.0f, 0.5f},
        {EnemyType::FAST, 30.0f, 40.0f, 0.5f},
        {EnemyType::TANK, 50.0f, 60.0f, 0.0f},
        {EnemyType::BOSS, 70.0f, 80.0f, 0.0f},
        {static_cast<EnemyType>(99), 90.0f, 100.0f, 0.0f}
    };
    level.waves = {wave};

    Entity entity = createLevelEntity(*engine, level);

    system.onUpdate(0.4f);
    EXPECT_TRUE(engine->getNetworkedEntities().empty());

    system.onUpdate(0.2f);
    EXPECT_EQ(engine->getNetworkedEntities().size(), 1u);

    auto firstId = *engine->getNetworkedEntities().begin();
    auto& firstVel = engine->getComponents<Velocity>()[firstId].value();
    auto& firstHealth = engine->getComponents<Health>()[firstId].value();
    EXPECT_FLOAT_EQ(firstVel.vx, BASE_ENEMY_VELOCITY_X);
    EXPECT_FLOAT_EQ(firstVel.vy, BASE_ENEMY_VELOCITY_Y);
    EXPECT_EQ(firstHealth.currentHealth, BASE_ENEMY_HEALTH_START);

    system.onUpdate(0.6f);
    EXPECT_EQ(engine->getNetworkedEntities().size(), 5u);

    bool hasFast = false;
    bool hasTank = false;
    for (auto id : engine->getNetworkedEntities()) {
        auto& vel = engine->getComponents<Velocity>()[id].value();
        auto& health = engine->getComponents<Health>()[id].value();
        if (vel.vx == FAST_ENEMY_VELOCITY_X && vel.vy == FAST_ENEMY_VELOCITY_Y && health.currentHealth == FAST_ENEMY_HEALTH) {
            hasFast = true;
        }
        if (vel.vx == TANK_ENEMY_VELOCITY_X && vel.vy == TANK_ENEMY_VELOCITY_Y && health.currentHealth == TANK_ENEMY_HEALTH) {
            hasTank = true;
        }
    }

    EXPECT_TRUE(hasFast);
    EXPECT_TRUE(hasTank);

    auto& updated = engine->getComponents<Level>()[static_cast<size_t>(entity)].value();
    EXPECT_TRUE(updated.completed);

    system.onUpdate(0.1f);
    EXPECT_EQ(engine->getNetworkedEntities().size(), 5u);
}
