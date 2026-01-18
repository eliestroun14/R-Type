#include <gtest/gtest.h>

#include <engine/ecs/entity/EntityManager.hpp>
#include <engine/ecs/system/SystemManager.hpp>
#include <engine/ecs/Signature.hpp>
#include <engine/ecs/component/Components.hpp>
#include <common/error/Error.hpp>

namespace {

void registerAllComponents(EntityManager& em)
{
    em.registerComponent<Transform>();
    em.registerComponent<Velocity>();
    em.registerComponent<Health>();
    em.registerComponent<Weapon>();
    em.registerComponent<AI>();
    em.registerComponent<HitBox>();
    em.registerComponent<Sprite>();
    em.registerComponent<Animation>();
    em.registerComponent<Powerup>();
    em.registerComponent<InputComponent>();
}

} // namespace

TEST(EntityManagerCoverage, RegisterAndGetComponentsErrors)
{
    EntityManager em;
    EXPECT_THROW(em.getComponents<Transform>(), Error);

    em.registerComponent<Transform>();
    EXPECT_NO_THROW(em.getComponents<Transform>());

    const EntityManager& constEm = em;
    EXPECT_NO_THROW(constEm.getComponents<Transform>());
}

TEST(EntityManagerCoverage, SpawnEntityWithIdThrowsWhenAlive)
{
    EntityManager em;
    Entity entity = em.spawnEntity("existing");
    std::size_t id = static_cast<std::size_t>(entity);

    EXPECT_THROW(em.spawnEntityWithId(id, "duplicate"), Error);
}

TEST(EntityManagerCoverage, SignatureSetAndGet)
{
    EntityManager em;
    Entity entity = em.spawnEntity("sig");

    Signature signature;
    signature.set(1, true);
    em.setSignature(entity, signature);

    Signature fetched = em.getSignature(entity);
    EXPECT_TRUE(fetched.test(1));
}

TEST(EntityManagerCoverage, UpdateComponentAndErrors)
{
    EntityManager em;
    registerAllComponents(em);

    Entity entity = em.spawnEntity("update");
    em.addComponent(entity, Transform(1.0f, 2.0f, 3.0f, 4.0f));

    em.updateComponent(entity, Transform(9.0f, 8.0f, 7.0f, 6.0f));
    auto& opt = em.getComponent<Transform>(entity);
    ASSERT_TRUE(opt.has_value());
    EXPECT_FLOAT_EQ(opt->x, 9.0f);

    em.killEntity(entity);
    EXPECT_THROW(em.updateComponent(entity, Transform(1.0f, 1.0f, 1.0f, 1.0f)), Error);
    EXPECT_THROW(em.addComponent(entity, Velocity(1.0f, 1.0f)), Error);
    EXPECT_THROW(em.removeComponent<Transform>(entity), Error);
}

TEST(EntityManagerCoverage, RemoveComponentByTypeSwitch)
{
    EntityManager em;
    registerAllComponents(em);

    Entity entity = em.spawnEntity("types");
    em.addComponent(entity, Transform(0.0f, 0.0f, 0.0f, 1.0f));
    em.addComponent(entity, Velocity(1.0f, 1.0f));
    em.addComponent(entity, Health(5, 10));
    em.addComponent(entity, Weapon(100, 0, 1, ProjectileType::LASER));
    em.addComponent(entity, AI(AiBehaviour::KAMIKAZE, 1.0f, 2.0f));
    em.addComponent(entity, HitBox{});
    em.addComponent(entity, Sprite(Assets::LOGO_RTYPE, ZIndex::IS_UI_HUD));
    em.addComponent(entity, Animation(1, 1, 0, 0.0f, 0.1f, 0, 1, true));
    em.addComponent(entity, Powerup(PowerupType::HEAL, 1.0f));
    em.addComponent(entity, InputComponent(1));

    em.removeComponentByType(0x01, entity);
    EXPECT_FALSE(em.hasComponent<Transform>(entity));

    em.removeComponentByType(0x02, entity);
    EXPECT_FALSE(em.hasComponent<Velocity>(entity));

    em.removeComponentByType(0x03, entity);
    EXPECT_FALSE(em.hasComponent<Health>(entity));

    em.removeComponentByType(0x04, entity);
    EXPECT_FALSE(em.hasComponent<Weapon>(entity));

    em.removeComponentByType(0x05, entity);
    EXPECT_FALSE(em.hasComponent<AI>(entity));

    em.removeComponentByType(0x07, entity);
    EXPECT_FALSE(em.hasComponent<HitBox>(entity));

    em.removeComponentByType(0x08, entity);
    EXPECT_FALSE(em.hasComponent<Sprite>(entity));

    em.removeComponentByType(0x09, entity);
    EXPECT_FALSE(em.hasComponent<Animation>(entity));

    em.removeComponentByType(0x0A, entity);
    EXPECT_FALSE(em.hasComponent<Powerup>(entity));

    em.removeComponentByType(0x0C, entity);
    EXPECT_FALSE(em.hasComponent<InputComponent>(entity));

    EXPECT_NO_THROW(em.removeComponentByType(0xFF, entity));
}

TEST(EntityManagerCoverage, KillEntityNoopWhenDead)
{
    EntityManager em;
    Entity ghost = Entity::fromId(999);

    EXPECT_NO_THROW(em.killEntity(ghost));
}
