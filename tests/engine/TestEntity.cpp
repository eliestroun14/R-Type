/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** test_entity
*/

#include <gtest/gtest.h>
#include "../../src/engine/include/engine/gameEngine/coordinator/ecs/entity/EntityManager.hpp"

TEST(EntityTest, ImplicitConversion) {
    EntityManager em;
    Entity e = em.spawnEntity("Test");

    size_t id = e; // Test implicit conversion
    EXPECT_GE(id, 0);
}

TEST(EntityTest, MultipleEntitiesHaveUniqueIds) {
    EntityManager em;
    Entity e1 = em.spawnEntity("First");
    Entity e2 = em.spawnEntity("Second");
    Entity e3 = em.spawnEntity("Third");

    size_t id1 = e1;
    size_t id2 = e2;
    size_t id3 = e3;

    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
}