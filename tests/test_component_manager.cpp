/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** test_component_manager
*/

#include <gtest/gtest.h>
#include "../src/engine/include/engine/ecs/Component.hpp"

struct Health {
    int current;
    int max;
    Health(int c, int m) : current(c), max(m) {}
};

TEST(ComponentManagerTest, InsertAt) {
    ComponentManager<Health> mgr;

    mgr.insertAt(0, Health{100, 100});

    ASSERT_TRUE(mgr[0].has_value());
    EXPECT_EQ(mgr[0]->current, 100);
    EXPECT_EQ(mgr[0]->max, 100);
}

TEST(ComponentManagerTest, Size) {
    ComponentManager<Health> mgr;

    mgr.emplaceAt(5, 50, 100);

    EXPECT_EQ(mgr.size(), 6);
}

TEST(ComponentManagerTest, EmplaceAt) {
    ComponentManager<Health> mgr;

    mgr.emplaceAt(5, 50, 100);

    EXPECT_EQ(mgr.size(), 6);
    ASSERT_TRUE(mgr[5].has_value());
    EXPECT_EQ(mgr[5]->current, 50);
}

TEST(ComponentManagerTest, Erase) {
    ComponentManager<Health> mgr;

    mgr.emplaceAt(0, 100, 100);
    EXPECT_TRUE(mgr[0].has_value());

    mgr.erase(0);
    EXPECT_FALSE(mgr[0].has_value());
}

TEST(ComponentManagerTest, Iteration) {
    ComponentManager<Health> mgr;

    mgr.emplaceAt(0, 100, 100);
    mgr.emplaceAt(2, 50, 50);
    mgr.emplaceAt(4, 75, 75);

    int count = 0;
    for (auto& health : mgr) {
        if (health.has_value()) {
            count++;
        }
    }

    EXPECT_EQ(count, 3);
}