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
        engine.Init();
        engine.RegisterComponent<Transform>();
    }

    GameEngine engine;
};

TEST_F(GameEngineTest, CreateEntity) {
    Entity e = engine.CreateEntity("TestEntity");
    EXPECT_TRUE(engine.IsAlive(e));
    EXPECT_EQ(engine.GetEntityName(e), "TestEntity");
}

TEST_F(GameEngineTest, DestroyEntity) {
    Entity e = engine.CreateEntity("TestEntity");
    engine.DestroyEntity(e);
    EXPECT_FALSE(engine.IsAlive(e));
}

TEST_F(GameEngineTest, AddAndAccessComponent) {
    Entity e = engine.CreateEntity("TestEntity");
    engine.EmplaceComponent<Transform>(e, 10.f, 20.f, 45.f);

    auto& transform = engine.GetComponentEntity<Transform>(e);
    ASSERT_TRUE(transform.has_value());
    EXPECT_FLOAT_EQ(transform->x, 10.f);
    EXPECT_FLOAT_EQ(transform->y, 20.f);
    EXPECT_FLOAT_EQ(transform->rotation, 45.f);
}

TEST_F(GameEngineTest, RemoveComponent) {
    Entity e = engine.CreateEntity("TestEntity");
    engine.EmplaceComponent<Transform>(e, 10.f, 20.f, 0.f);
    engine.RemoveComponent<Transform>(e);

    auto& transform = engine.GetComponentEntity<Transform>(e);
    EXPECT_FALSE(transform.has_value());
}

TEST_F(GameEngineTest, IterateComponents) {
    Entity e1 = engine.CreateEntity("A");
    Entity e2 = engine.CreateEntity("B");
    Entity e3 = engine.CreateEntity("C");

    engine.EmplaceComponent<Transform>(e1, 1.f, 2.f, 0.f);
    engine.EmplaceComponent<Transform>(e2, 3.f, 4.f, 0.f);
    engine.EmplaceComponent<Transform>(e3, 5.f, 6.f, 0.f);

    auto& transforms = engine.GetComponents<Transform>();

    int count = 0;
    for (size_t id = 0; id < transforms.size(); ++id) {
        if (transforms[id].has_value()) {
            count++;
        }
    }

    EXPECT_EQ(count, 3);
}