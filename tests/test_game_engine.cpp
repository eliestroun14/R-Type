/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** test_game_engine
*/

#include <gtest/gtest.h>
#include "GameEngine.hpp"

struct Transform {
    float x, y, rotation;
    Transform(float xx, float yy, float rot)
        : x(xx), y(yy), rotation(rot) {}
};

class GameEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine.init();
        engine.registerComponent<Transform>();
    }

    GameEngine engine;
};

TEST_F(GameEngineTest, createEntity) {
    Entity e = engine.createEntity("TestEntity");
    EXPECT_TRUE(engine.isAlive(e));
    EXPECT_EQ(engine.getEntityName(e), "TestEntity");
}

TEST_F(GameEngineTest, destroyEntity) {
    Entity e = engine.createEntity("TestEntity");
    engine.destroyEntity(e);
    EXPECT_FALSE(engine.isAlive(e));
}

TEST_F(GameEngineTest, AddAndAccessComponent) {
    Entity e = engine.createEntity("TestEntity");
    engine.emplaceComponent<Transform>(e, 10.f, 20.f, 45.f);

    auto& transform = engine.getComponentEntity<Transform>(e);
    ASSERT_TRUE(transform.has_value());
    EXPECT_FLOAT_EQ(transform->x, 10.f);
    EXPECT_FLOAT_EQ(transform->y, 20.f);
    EXPECT_FLOAT_EQ(transform->rotation, 45.f);
}

TEST_F(GameEngineTest, removeComponent) {
    Entity e = engine.createEntity("TestEntity");
    engine.emplaceComponent<Transform>(e, 10.f, 20.f, 0.f);
    engine.removeComponent<Transform>(e);

    auto& transform = engine.getComponentEntity<Transform>(e);
    EXPECT_FALSE(transform.has_value());
}

TEST_F(GameEngineTest, IterateComponents) {
    Entity e1 = engine.createEntity("A");
    Entity e2 = engine.createEntity("B");
    Entity e3 = engine.createEntity("C");

    engine.emplaceComponent<Transform>(e1, 1.f, 2.f, 0.f);
    engine.emplaceComponent<Transform>(e2, 3.f, 4.f, 0.f);
    engine.emplaceComponent<Transform>(e3, 5.f, 6.f, 0.f);

    auto& transforms = engine.getComponents<Transform>();

    int count = 0;
    for (size_t id = 0; id < transforms.size(); ++id) {
        if (transforms[id].has_value()) {
            count++;
        }
    }

    EXPECT_EQ(count, 3);
}