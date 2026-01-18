#include <gtest/gtest.h>

#define private public
#include <game/coordinator/Coordinator.hpp>
#include <game/systems/ShootSystem.hpp>
#undef private

#include <engine/ecs/component/Components.hpp>
#include <common/protocol/Protocol.hpp>

namespace {

Coordinator makeCoordinator(bool isServer)
{
    Coordinator coord;
    coord.setIsServer(isServer);
    coord.initEngine();
    return coord;
}

Entity createShooter(gameEngine::GameEngine& engine,
                     const char* name,
                     const Transform& transform,
                     const Weapon& weapon)
{
    Entity entity = engine.createEntity(name);
    engine.addComponent(entity, Transform(transform));
    engine.addComponent(entity, Weapon(weapon));
    return entity;
}

} // namespace

TEST(ShootSystemCoverage, OnStartRunningDoesNotThrow)
{
    Coordinator coord = makeCoordinator(false);
    auto engine = coord.getEngine();
    auto& system = engine->getSystem<ShootSystem>();

    EXPECT_NO_THROW(system.onStartRunning());
}

TEST(ShootSystemCoverage, ClientDoesNotQueueWeaponFire)
{
    Coordinator coord = makeCoordinator(false);
    auto engine = coord.getEngine();
    auto& system = engine->getSystem<ShootSystem>();

    Entity player = createShooter(*engine, "player",
        Transform(10.0f, 20.0f, 0.0f, 1.0f),
        Weapon(0, 0, 5, ProjectileType::MISSILE));

    engine->addComponent(player, InputComponent(1));
    auto& input = engine->getComponents<InputComponent>()[static_cast<size_t>(player)].value();
    input.activeActions[GameAction::SHOOT] = true;

    system.onUpdate(0.1f);

    EXPECT_TRUE(coord._pendingWeaponFires.empty());
    auto& weapon = engine->getComponents<Weapon>()[static_cast<size_t>(player)].value();
    EXPECT_GT(weapon.lastShotTime, 0u);
}

TEST(ShootSystemCoverage, ServerQueuesWeaponFireUsesClientPosition)
{
    Coordinator coord = makeCoordinator(true);
    auto engine = coord.getEngine();
    auto& system = engine->getSystem<ShootSystem>();

    Entity player = createShooter(*engine, "player",
        Transform(10.0f, 20.0f, 0.0f, 1.0f),
        Weapon(0, 0, 5, ProjectileType::MISSILE));

    engine->addComponent(player, InputComponent(7));
    engine->addComponent(player, NetworkId(123));
    auto& input = engine->getComponents<InputComponent>()[static_cast<size_t>(player)].value();
    input.activeActions[GameAction::SHOOT] = true;
    input.clientPosX = 99.0f;
    input.clientPosY = 88.0f;

    system.onUpdate(0.1f);

    ASSERT_EQ(coord._pendingWeaponFires.size(), 1u);
    const auto& event = coord._pendingWeaponFires.back();
    EXPECT_EQ(event.shooterId, 123u);
    EXPECT_FLOAT_EQ(event.originX, 99.0f);
    EXPECT_FLOAT_EQ(event.originY, 88.0f);
    EXPECT_FLOAT_EQ(event.directionX, 1.0f);
    EXPECT_FLOAT_EQ(event.directionY, 0.0f);
    EXPECT_EQ(event.weaponType, static_cast<uint8_t>(protocol::WeaponTypes::WEAPON_TYPE_BASIC));
}

TEST(ShootSystemCoverage, EnemyShootsLeftOnServer)
{
    Coordinator coord = makeCoordinator(true);
    auto engine = coord.getEngine();
    auto& system = engine->getSystem<ShootSystem>();

    Entity enemy = createShooter(*engine, "enemy",
        Transform(0.0f, 0.0f, 0.0f, 1.0f),
        Weapon(0, 0, 5, ProjectileType::LASER));

    system.onUpdate(0.1f);

    ASSERT_EQ(coord._pendingWeaponFires.size(), 1u);
    const auto& event = coord._pendingWeaponFires.back();
    EXPECT_LT(event.directionX, 0.0f);
    EXPECT_FLOAT_EQ(event.directionY, 0.0f);
}

TEST(ShootSystemCoverage, HelperMethodsWork)
{
    Coordinator coord = makeCoordinator(true);
    auto engine = coord.getEngine();
    auto& system = engine->getSystem<ShootSystem>();

    Entity player = engine->createEntity("player");
    engine->addComponent(player, InputComponent(1));
    engine->addComponent(player, Transform(0.0f, 0.0f, 0.0f, 1.0f));

    Entity enemy = engine->createEntity("enemy");
    engine->addComponent(enemy, Transform(0.0f, 0.0f, 0.0f, 1.0f));

    auto playerDir = system.calculateDirection(player);
    auto enemyDir = system.calculateDirection(enemy);

    EXPECT_FLOAT_EQ(playerDir.first, 1.0f);
    EXPECT_FLOAT_EQ(enemyDir.first, -1.0f);

    Weapon weapon(100, 0, 5, ProjectileType::MISSILE);
    EXPECT_TRUE(system.canShoot(weapon, weapon.fireRateMs));

    EXPECT_EQ(system.getProjectileAsset(ProjectileType::MISSILE, true), DEFAULT_BULLET);
    EXPECT_EQ(system.getProjectileAsset(ProjectileType::LASER, false), CHARCHING_BULLET);
    EXPECT_EQ(system.getProjectileAsset(ProjectileType::UNKNOWN, false), DEFAULT_BULLET);
}
