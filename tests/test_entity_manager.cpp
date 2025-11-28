/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** test_entity_manager
*/

#include <gtest/gtest.h>
#include "ECS/Entity/EntityManager.hpp"

struct Position {
    float x, y;
    Position(float xx, float yy) : x(xx), y(yy) {}
};

struct Velocity {
    float vx, vy;
    Velocity(float vxx, float vyy) : vx(vxx), vy(vyy) {}
};

// Test Fixture
class EntityManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        em.registerComponent<Position>();
        em.registerComponent<Velocity>();
    }

    EntityManager em;
};

TEST_F(EntityManagerTest, SpawnEntity) {
    Entity e = em.spawnEntity("TestEntity");
    EXPECT_TRUE(em.isAlive(e));
}

TEST_F(EntityManagerTest, KillEntityWithNoComponent) {
    Entity e = em.spawnEntity("TestEntity");
    em.killEntity(e);
    EXPECT_FALSE(em.isAlive(e));
}

TEST_F(EntityManagerTest, KillEntityWithComponent) {
    Entity e = em.spawnEntity("TestEntity");
    em.emplaceComponent<Position>(e, 10.f, 20.f);
    em.killEntity(e);
    EXPECT_FALSE(em.isAlive(e));
}

TEST_F(EntityManagerTest, addComponent) {
    Entity e = em.spawnEntity("TestEntity");
    em.emplaceComponent<Position>(e, 10.f, 20.f);

    auto& pos = em.getComponent<Position>(e);
    ASSERT_TRUE(pos.has_value());
    EXPECT_FLOAT_EQ(pos->x, 10.f);
    EXPECT_FLOAT_EQ(pos->y, 20.f);
}

TEST_F(EntityManagerTest, removeComponent) {
    Entity e = em.spawnEntity("TestEntity");
    em.emplaceComponent<Position>(e, 10.f, 20.f);

    EXPECT_TRUE(em.hasComponent<Position>(e));

    em.removeComponent<Position>(e);

    EXPECT_FALSE(em.hasComponent<Position>(e));
}

TEST_F(EntityManagerTest, MultipleComponents) {
    Entity e = em.spawnEntity("TestEntity");
    em.emplaceComponent<Position>(e, 5.f, 15.f);
    em.emplaceComponent<Velocity>(e, 1.f, 2.f);

    EXPECT_TRUE(em.hasComponent<Position>(e));
    EXPECT_TRUE(em.hasComponent<Velocity>(e));

    auto& pos = em.getComponent<Position>(e);
    auto& vel = em.getComponent<Velocity>(e);

    ASSERT_TRUE(pos.has_value());
    ASSERT_TRUE(vel.has_value());

    EXPECT_FLOAT_EQ(pos->x, 5.f);
    EXPECT_FLOAT_EQ(vel->vx, 1.f);
}

TEST_F(EntityManagerTest, EntityRecycling) {
    Entity e1 = em.spawnEntity("First");
    size_t id1 = static_cast<size_t>(e1);

    em.killEntity(e1);

    Entity e2 = em.spawnEntity("Second");
    size_t id2 = static_cast<size_t>(e2);

    // L'ID devrait être recyclé
    EXPECT_EQ(id1, id2);
}