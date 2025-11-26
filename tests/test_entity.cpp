/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** test_entity
*/

#include <gtest/gtest.h>
#include "ECS/Entity/Entity.hpp"
#include "ECS/Entity/EntityManager.hpp"

TEST(EntityTest, ImplicitConversion) {
    EntityManager em;
    Entity e = em.spawn_entity("Test");

    size_t id = e; // Test implicit conversion
    EXPECT_GE(id, 0);
}

TEST(EntityTest, MultipleEntitiesHaveUniqueIds) {
    EntityManager em;
    Entity e1 = em.spawn_entity("First");
    Entity e2 = em.spawn_entity("Second");
    Entity e3 = em.spawn_entity("Third");

    size_t id1 = e1;
    size_t id2 = e2;
    size_t id3 = e3;

    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
}