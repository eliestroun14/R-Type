/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** test_system_manager
*/

#include <gtest/gtest.h>
#include "System/SystemManager.hpp"
#include "System/System.hpp"
#include "Entity/EntityManager.hpp"

class DummySystem : public System {
public:
    DummySystem() = default;

    void onCreate() override { created = true; }
    void onDestroy() override { destroyed = true; }
    void onUpdate(float dt) override {
        lastDt = dt;
        ++updateCount;
    }

    bool created {false};
    bool destroyed {false};
    int updateCount {0};
    float lastDt {0.f};
};

class AnotherSystem : public System {
public:
    void onCreate() override { created = true; }
    void onDestroy() override { destroyed = true; }
    void onUpdate(float) override { ++updateCount; }

    bool created {false};
    bool destroyed {false};
    int updateCount {0};
};

TEST(SystemManagerTest, AddSystemAndHasSystem)
{
    SystemManager manager;

    auto &sys = manager.addSystem<DummySystem>();

    EXPECT_TRUE(manager.hasSystem<DummySystem>());

    EXPECT_TRUE(sys.empty());

    EntityManager em;
    Entity e = em.spawnEntity("dummy");
    sys.addEntity(e);

    EXPECT_FALSE(sys.empty());
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

    EXPECT_THROW(manager.getSystem<DummySystem>(), std::runtime_error);
}

TEST(SystemManagerTest, DeleteSystemRemovesSystem)
{
    SystemManager manager;

    manager.addSystem<DummySystem>();
    EXPECT_TRUE(manager.hasSystem<DummySystem>());

    manager.deleteSystem<DummySystem>();
    EXPECT_FALSE(manager.hasSystem<DummySystem>());
    EXPECT_THROW(manager.getSystem<DummySystem>(), std::runtime_error);
}

TEST(SystemManagerTest, OnCreateAllCallsOnCreate)
{
    SystemManager manager;

    auto &sys1 = manager.addSystem<DummySystem>();
    auto &sys2 = manager.addSystem<AnotherSystem>();

    EXPECT_FALSE(sys1.created);
    EXPECT_FALSE(sys2.created);

    manager.onCreateAll();

    EXPECT_TRUE(sys1.created);
    EXPECT_TRUE(sys2.created);
}

TEST(SystemManagerTest, OnDestroyAllCallsOnDestroy)
{
    SystemManager manager;

    auto &sys1 = manager.addSystem<DummySystem>();
    auto &sys2 = manager.addSystem<AnotherSystem>();

    EXPECT_FALSE(sys1.destroyed);
    EXPECT_FALSE(sys2.destroyed);

    manager.onDestroyAll();

    EXPECT_TRUE(sys1.destroyed);
    EXPECT_TRUE(sys2.destroyed);
}

TEST(SystemManagerTest, UpdateAllCallsOnUpdateWhenNotEmpty)
{
    SystemManager manager;

    auto &sys = manager.addSystem<DummySystem>();

    EntityManager em;
    Entity e = em.spawnEntity("dummy");
    sys.addEntity(e);

    EXPECT_EQ(sys.updateCount, 0);

    float dt = 0.016f;
    manager.updateAll(dt);

    EXPECT_EQ(sys.updateCount, 1);
    EXPECT_FLOAT_EQ(sys.lastDt, dt);
}

TEST(SystemManagerTest, UpdateAllSkipsEmptySystems)
{
    SystemManager manager;

    auto &sys = manager.addSystem<DummySystem>();

    EXPECT_TRUE(sys.empty());

    manager.updateAll(0.1f);

    EXPECT_EQ(sys.updateCount, 0);
}
