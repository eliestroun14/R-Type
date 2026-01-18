/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BackgroundSystem coverage tests
*/

#include <gtest/gtest.h>

#define private public
#define protected public
#include <game/systems/BackgroundSystem.hpp>
#undef protected
#undef private

TEST(BackgroundSystemCoverage, ServerModeEarlyReturn) {
    gameEngine::GameEngine engine;
    BackgroundSystem system(engine, true);

    auto entity = engine.createEntity("Bg");
    engine.addComponent<Transform>(entity, Transform(10.0f, 20.0f, 0.0f, 1.0f));
    engine.addComponent<Sprite>(entity, Sprite(DEFAULT_NONE_BUTTON, ZIndex::IS_BACKGROUND, sf::IntRect(0, 0, 100, 50)));
    engine.addComponent<ScrollingBackground>(entity, ScrollingBackground(5.0f, true, true));

    system._entities = {static_cast<size_t>(entity)};

    auto& bg = engine.getComponentEntity<ScrollingBackground>(entity);
    bg->currentOffset = -95.0f;

    system.onUpdate(1.0f);

    auto& transform = engine.getComponentEntity<Transform>(entity);
    EXPECT_FLOAT_EQ(transform->x, 10.0f);
    EXPECT_FLOAT_EQ(bg->currentOffset, -95.0f);
}

TEST(BackgroundSystemCoverage, HorizontalRepeatWrapsOffset) {
    gameEngine::GameEngine engine;
    BackgroundSystem system(engine, false);

    auto entity = engine.createEntity("Bg");
    engine.addComponent<Transform>(entity, Transform(0.0f, 0.0f, 0.0f, 1.0f));
    engine.addComponent<Sprite>(entity, Sprite(DEFAULT_NONE_BUTTON, ZIndex::IS_BACKGROUND, sf::IntRect(0, 0, 100, 50)));
    engine.addComponent<ScrollingBackground>(entity, ScrollingBackground(10.0f, true, true));

    system._entities = {static_cast<size_t>(entity)};

    auto& bg = engine.getComponentEntity<ScrollingBackground>(entity);
    bg->currentOffset = -95.0f;

    system.onUpdate(1.0f);

    auto& transform = engine.getComponentEntity<Transform>(entity);
    EXPECT_FLOAT_EQ(bg->currentOffset, -5.0f);
    EXPECT_FLOAT_EQ(transform->x, -5.0f);
}

TEST(BackgroundSystemCoverage, VerticalNoRepeatUpdatesY) {
    gameEngine::GameEngine engine;
    BackgroundSystem system(engine, false);

    engine.setScaleFactor(2.0f);

    auto entity = engine.createEntity("Bg");
    engine.addComponent<Transform>(entity, Transform(0.0f, 0.0f, 0.0f, 1.0f));
    engine.addComponent<Sprite>(entity, Sprite(DEFAULT_NONE_BUTTON, ZIndex::IS_BACKGROUND, sf::IntRect(0, 0, 10, 20)));
    engine.addComponent<ScrollingBackground>(entity, ScrollingBackground(2.5f, false, false));

    system._entities = {static_cast<size_t>(entity)};

    system.onUpdate(1.0f);

    auto& bg = engine.getComponentEntity<ScrollingBackground>(entity);
    auto& transform = engine.getComponentEntity<Transform>(entity);

    EXPECT_FLOAT_EQ(bg->currentOffset, -5.0f);
    EXPECT_FLOAT_EQ(transform->y, -5.0f);
}

TEST(BackgroundSystemCoverage, SkipsEntitiesMissingComponents) {
    gameEngine::GameEngine engine;
    BackgroundSystem system(engine, false);

    auto entity = engine.createEntity("Bg");
    engine.addComponent<Transform>(entity, Transform(1.0f, 2.0f, 0.0f, 1.0f));

    system._entities = {static_cast<size_t>(entity)};

    system.onUpdate(1.0f);

    auto& transform = engine.getComponentEntity<Transform>(entity);
    EXPECT_FLOAT_EQ(transform->x, 1.0f);
    EXPECT_FLOAT_EQ(transform->y, 2.0f);
}
