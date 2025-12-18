/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** test_system_manager
*/

#include <gtest/gtest.h>
#include <engine/ecs/system/SystemManager.hpp>
#include <engine/ecs/system/System.hpp>
#include <engine/ecs/entity/EntityManager.hpp>

struct Position {
    float x, y;
    Position(float xx, float yy) : x(xx), y(yy) {}
};

class DummySystem : public System {
public:
    void onCreate() override { created = true; }
    void onDestroy() override { destroyed = true; }
    void onUpdate(float dt) override { lastDt = dt; ++updateCount; }

    bool created = false;
    bool destroyed = false;
    int updateCount = 0;
    float lastDt = 0.f;
};

class AnotherSystem : public System {
public:
    void onCreate() override { created = true; }
    void onDestroy() override { destroyed = true; }
    void onUpdate(float) override { ++updateCount; }

    bool created = false;
    bool destroyed = false;
    int updateCount = 0;
};

TEST(SystemManagerTest, AddSystemAndHasSystem)
{
    SystemManager manager;
    EntityManager em;
    em.setSystemManager(&manager);

    auto &sys = manager.addSystem<DummySystem>();
    EXPECT_TRUE(manager.hasSystem<DummySystem>());
    EXPECT_EQ(&sys, &manager.getSystem<DummySystem>());

    em.registerComponent<Position>();

    Signature sig;
    sig.set(0);
    manager.setSignature<DummySystem>(sig);

    Entity e = em.spawnEntity("dummy");
    em.emplaceComponent<Position>(e, 1.f, 2.f);

    EXPECT_EQ(sys.entityCount(), 1);
    EXPECT_TRUE(sys.hasEntity(static_cast<size_t>(e)));

}

TEST(SystemManagerTest, GetSystemReturnsSameInstance)
{
    SystemManager manager;

    auto &sys1 = manager.addSystem<DummySystem>();
    auto &sys2 = manager.getSystem<DummySystem>();

    EXPECT_EQ(&sys1, &sys2);
}

TEST(SystemManagerTest, GetConstSystemReturnsSameInstance)
{
    SystemManager manager;

    auto &sys = manager.addSystem<DummySystem>();
    const SystemManager &cmanager = manager;

    const auto &csys = cmanager.getSystem<DummySystem>();

    EXPECT_EQ(&sys, &csys);
}

TEST(SystemManagerTest, GetSystemThrowsIfNotFound)
{
    SystemManager manager;

    EXPECT_THROW(manager.getSystem<DummySystem>(), Error);
}

TEST(SystemManagerTest, DeleteSystemRemovesSystem)
{
    SystemManager manager;

    manager.addSystem<DummySystem>();
    EXPECT_TRUE(manager.hasSystem<DummySystem>());

    manager.deleteSystem<DummySystem>();
    EXPECT_FALSE(manager.hasSystem<DummySystem>());
    EXPECT_THROW(manager.getSystem<DummySystem>(), Error);
}

TEST(SystemManagerTest, OnCreateAllCallsOnCreate)
{
    SystemManager manager;

    auto &sys1 = manager.addSystem<DummySystem>();
    auto &sys2 = manager.addSystem<AnotherSystem>();

    manager.onCreateAll();

    EXPECT_TRUE(sys1.created);
    EXPECT_TRUE(sys2.created);
}

TEST(SystemManagerTest, OnDestroyAllCallsOnDestroy)
{
    SystemManager manager;

    auto &sys1 = manager.addSystem<DummySystem>();
    auto &sys2 = manager.addSystem<AnotherSystem>();

    manager.onDestroyAll();

    EXPECT_TRUE(sys1.destroyed);
    EXPECT_TRUE(sys2.destroyed);
}

TEST(SystemManagerTest, UpdateAllCallsOnUpdate)
{
    SystemManager manager;

    auto &sys = manager.addSystem<DummySystem>();

    EXPECT_EQ(sys.updateCount, 0);

    float dt = 0.016f;
    manager.updateAll(dt);

    EXPECT_EQ(sys.updateCount, 1);
    EXPECT_FLOAT_EQ(sys.lastDt, dt);
}
