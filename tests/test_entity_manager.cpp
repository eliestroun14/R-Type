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
        em.register_component<Position>();
        em.register_component<Velocity>();
    }

    EntityManager em;
};

TEST_F(EntityManagerTest, SpawnEntity) {
    Entity e = em.spawn_entity("TestEntity");
    EXPECT_TRUE(em.is_alive(e));
}

TEST_F(EntityManagerTest, KillEntityWithNoComponent) {
    Entity e = em.spawn_entity("TestEntity");
    em.kill_entity(e);
    EXPECT_FALSE(em.is_alive(e));
}

TEST_F(EntityManagerTest, KillEntityWithComponent) {
    Entity e = em.spawn_entity("TestEntity");
    em.emplace_component<Position>(e, 10.f, 20.f);
    em.kill_entity(e);
    EXPECT_FALSE(em.is_alive(e));
}

TEST_F(EntityManagerTest, AddComponent) {
    Entity e = em.spawn_entity("TestEntity");
    em.emplace_component<Position>(e, 10.f, 20.f);

    auto& pos = em.get_component<Position>(e);
    ASSERT_TRUE(pos.has_value());
    EXPECT_FLOAT_EQ(pos->x, 10.f);
    EXPECT_FLOAT_EQ(pos->y, 20.f);
}

TEST_F(EntityManagerTest, RemoveComponent) {
    Entity e = em.spawn_entity("TestEntity");
    em.emplace_component<Position>(e, 10.f, 20.f);

    EXPECT_TRUE(em.has_component<Position>(e));

    em.remove_component<Position>(e);

    EXPECT_FALSE(em.has_component<Position>(e));
}

TEST_F(EntityManagerTest, MultipleComponents) {
    Entity e = em.spawn_entity("TestEntity");
    em.emplace_component<Position>(e, 5.f, 15.f);
    em.emplace_component<Velocity>(e, 1.f, 2.f);

    EXPECT_TRUE(em.has_component<Position>(e));
    EXPECT_TRUE(em.has_component<Velocity>(e));

    auto& pos = em.get_component<Position>(e);
    auto& vel = em.get_component<Velocity>(e);

    ASSERT_TRUE(pos.has_value());
    ASSERT_TRUE(vel.has_value());

    EXPECT_FLOAT_EQ(pos->x, 5.f);
    EXPECT_FLOAT_EQ(vel->vx, 1.f);
}

TEST_F(EntityManagerTest, EntityRecycling) {
    Entity e1 = em.spawn_entity("First");
    size_t id1 = static_cast<size_t>(e1);

    em.kill_entity(e1);

    Entity e2 = em.spawn_entity("Second");
    size_t id2 = static_cast<size_t>(e2);

    // L'ID devrait être recyclé
    EXPECT_EQ(id1, id2);
}