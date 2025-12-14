/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** test_system
*/

#include <gtest/gtest.h>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>

class TestableSystem : public System {
public:
    void addEntity(size_t id) { _entities.push_back(id); }
    void removeEntity(size_t id) {
        _entities.erase(std::remove(_entities.begin(), _entities.end(), id), _entities.end());
    }

    using System::onCreate;
    using System::onStartRunning;
    using System::onUpdate;
    using System::onStopRunning;
    using System::onDestroy;

    void setRunning(bool r) { _running = r; }
};

TEST(SystemTest, StartsNotRunning)
{
    TestableSystem sys;
    EXPECT_FALSE(sys.isRunning());
}

TEST(SystemTest, EntityCountInitiallyZero)
{
    TestableSystem sys;
    EXPECT_EQ(sys.entityCount(), 0u);
}

TEST(SystemTest, AddEntitiesUpdatesCount)
{
    TestableSystem sys;

    sys.addEntity(1);
    sys.addEntity(42);

    EXPECT_EQ(sys.entityCount(), 2u);
}

TEST(SystemTest, HasEntityReturnsCorrectValue)
{
    TestableSystem sys;

    sys.addEntity(3);
    sys.addEntity(7);

    EXPECT_TRUE(sys.hasEntity(3));
    EXPECT_TRUE(sys.hasEntity(7));
    EXPECT_FALSE(sys.hasEntity(99));
}

TEST(SystemTest, RemoveEntityUpdatesState)
{
    TestableSystem sys;

    sys.addEntity(10);
    sys.addEntity(20);

    sys.removeEntity(10);

    EXPECT_FALSE(sys.hasEntity(10));
    EXPECT_TRUE(sys.hasEntity(20));
    EXPECT_EQ(sys.entityCount(), 1u);
}

TEST(SystemTest, RemoveNonExistingEntityDoesNothing)
{
    TestableSystem sys;

    sys.addEntity(5);

    sys.removeEntity(999);

    EXPECT_TRUE(sys.hasEntity(5));
    EXPECT_EQ(sys.entityCount(), 1u);
}

TEST(SystemTest, LifecycleMethodsAreCallable)
{
    TestableSystem sys;

    EXPECT_NO_THROW(sys.onCreate());
    EXPECT_NO_THROW(sys.onStartRunning());
    EXPECT_NO_THROW(sys.onUpdate(0.16f));
    EXPECT_NO_THROW(sys.onStopRunning());
    EXPECT_NO_THROW(sys.onDestroy());
}

TEST(SystemTest, RunningFlagIsControllable)
{
    TestableSystem sys;

    EXPECT_FALSE(sys.isRunning());
    sys.setRunning(true);
    EXPECT_TRUE(sys.isRunning());
    sys.setRunning(false);
    EXPECT_FALSE(sys.isRunning());
}
