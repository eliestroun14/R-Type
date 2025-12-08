/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** test_game_coordinator
*/

#include <gtest/gtest.h>
#include "../src/engine/include/engine/ecs/Coordinator.hpp"

struct Transform {
    float x, y, rotation;
    Transform(float xx, float yy, float rot)
        : x(xx), y(yy), rotation(rot) {}
};

class CoordinatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        coordinator.init();
        coordinator.registerComponent<Transform>();
    }

    Coordinator coordinator;
};

TEST_F(CoordinatorTest, createEntity) {
    Entity e = coordinator.createEntity("TestEntity");
    EXPECT_TRUE(coordinator.isAlive(e));
    EXPECT_EQ(coordinator.getEntityName(e), "TestEntity");
}

TEST_F(CoordinatorTest, destroyEntity) {
    Entity e = coordinator.createEntity("TestEntity");
    coordinator.destroyEntity(e);
    EXPECT_FALSE(coordinator.isAlive(e));
}

TEST_F(CoordinatorTest, AddAndAccessComponent) {
    Entity e = coordinator.createEntity("TestEntity");
    coordinator.emplaceComponent<Transform>(e, 10.f, 20.f, 45.f);

    auto& transform = coordinator.getComponentEntity<Transform>(e);
    ASSERT_TRUE(transform.has_value());
    EXPECT_FLOAT_EQ(transform->x, 10.f);
    EXPECT_FLOAT_EQ(transform->y, 20.f);
    EXPECT_FLOAT_EQ(transform->rotation, 45.f);
}

TEST_F(CoordinatorTest, removeComponent) {
    Entity e = coordinator.createEntity("TestEntity");
    coordinator.emplaceComponent<Transform>(e, 10.f, 20.f, 0.f);
    coordinator.removeComponent<Transform>(e);

    auto& transform = coordinator.getComponentEntity<Transform>(e);
    EXPECT_FALSE(transform.has_value());
}

TEST_F(CoordinatorTest, IterateComponents) {
    Entity e1 = coordinator.createEntity("A");
    Entity e2 = coordinator.createEntity("B");
    Entity e3 = coordinator.createEntity("C");

    coordinator.emplaceComponent<Transform>(e1, 1.f, 2.f, 0.f);
    coordinator.emplaceComponent<Transform>(e2, 3.f, 4.f, 0.f);
    coordinator.emplaceComponent<Transform>(e3, 5.f, 6.f, 0.f);

    auto& transforms = coordinator.getComponents<Transform>();

    int count = 0;
    for (size_t id = 0; id < transforms.size(); ++id) {
        if (transforms[id].has_value()) {
            count++;
        }
    }

    EXPECT_EQ(count, 3);
}