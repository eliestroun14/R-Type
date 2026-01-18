/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ButtonSystem coverage tests
*/

#include <gtest/gtest.h>

#define private public
#define protected public
#include <game/systems/ButtonSystem.hpp>
#undef protected
#undef private

TEST(ButtonSystemCoverage, ServerModeEarlyReturn) {
    gameEngine::GameEngine engine;
    ButtonSystem system(engine, true);

    auto entity = engine.createEntity("Button");
    engine.addComponent<Sprite>(entity, Sprite(DEFAULT_NONE_BUTTON, ZIndex::IS_UI_HUD, sf::IntRect(0, 0, 100, 50)));
    engine.addComponent<ButtonComponent>(entity, ButtonComponent({DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON}, []() {}));

    engine.setMousePosition(sf::Vector2i(10, 10));
    engine.setLeftClickHeld(true);

    system.onUpdate(0.016f);

    auto& button = engine.getComponentEntity<ButtonComponent>(entity);
    auto& sprite = engine.getComponentEntity<Sprite>(entity);

    EXPECT_EQ(button->state, ButtonState::NONE);
    EXPECT_EQ(sprite->assetId, DEFAULT_NONE_BUTTON);
}

TEST(ButtonSystemCoverage, HoverSetsHoverStateAndAsset) {
    gameEngine::GameEngine engine;
    ButtonSystem system(engine, false);

    auto entity = engine.createEntity("Button");
    engine.addComponent<Sprite>(entity, Sprite(DEFAULT_NONE_BUTTON, ZIndex::IS_UI_HUD, sf::IntRect(0, 0, 100, 50)));
    engine.addComponent<ButtonComponent>(entity, ButtonComponent({DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON}, []() {}));

    engine.setMousePosition(sf::Vector2i(10, 10));

    system.onUpdate(0.016f);

    auto& button = engine.getComponentEntity<ButtonComponent>(entity);
    auto& sprite = engine.getComponentEntity<Sprite>(entity);

    EXPECT_EQ(button->state, ButtonState::HOVER);
    EXPECT_EQ(sprite->assetId, DEFAULT_HOVER_BUTTON);
}

TEST(ButtonSystemCoverage, ClickHeldSetsClickedState) {
    gameEngine::GameEngine engine;
    ButtonSystem system(engine, false);

    auto entity = engine.createEntity("Button");
    engine.addComponent<Sprite>(entity, Sprite(DEFAULT_NONE_BUTTON, ZIndex::IS_UI_HUD, sf::IntRect(0, 0, 100, 50)));
    engine.addComponent<ButtonComponent>(entity, ButtonComponent({DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON}, []() {}));

    engine.setMousePosition(sf::Vector2i(10, 10));
    engine.setLeftClickHeld(true);

    system.onUpdate(0.016f);

    auto& button = engine.getComponentEntity<ButtonComponent>(entity);
    auto& sprite = engine.getComponentEntity<Sprite>(entity);

    EXPECT_EQ(button->state, ButtonState::CLICKED);
    EXPECT_EQ(sprite->assetId, DEFAULT_CLICKED_BUTTON);
}

TEST(ButtonSystemCoverage, ClickReleaseTriggersOnClick) {
    gameEngine::GameEngine engine;
    ButtonSystem system(engine, false);

    bool clicked = false;

    auto entity = engine.createEntity("Button");
    engine.addComponent<Sprite>(entity, Sprite(DEFAULT_NONE_BUTTON, ZIndex::IS_UI_HUD, sf::IntRect(0, 0, 100, 50)));
    engine.addComponent<ButtonComponent>(entity, ButtonComponent({DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON}, [&clicked]() {
        clicked = true;
    }));

    auto& button = engine.getComponentEntity<ButtonComponent>(entity);
    button->state = ButtonState::CLICKED;

    engine.setMousePosition(sf::Vector2i(10, 10));
    engine.setLeftClickReleased(true);

    system.onUpdate(0.016f);

    auto& sprite = engine.getComponentEntity<Sprite>(entity);
    EXPECT_TRUE(clicked);
    EXPECT_EQ(button->state, ButtonState::HOVER);
    EXPECT_EQ(sprite->assetId, DEFAULT_HOVER_BUTTON);
}

TEST(ButtonSystemCoverage, NonHoverSetsNoneState) {
    gameEngine::GameEngine engine;
    ButtonSystem system(engine, false);

    auto entity = engine.createEntity("Button");
    engine.addComponent<Sprite>(entity, Sprite(DEFAULT_HOVER_BUTTON, ZIndex::IS_UI_HUD, sf::IntRect(0, 0, 100, 50)));
    engine.addComponent<ButtonComponent>(entity, ButtonComponent({DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON}, []() {}));

    auto& button = engine.getComponentEntity<ButtonComponent>(entity);
    button->state = ButtonState::HOVER;

    engine.setMousePosition(sf::Vector2i(200, 200));

    system.onUpdate(0.016f);

    auto& sprite = engine.getComponentEntity<Sprite>(entity);

    EXPECT_EQ(button->state, ButtonState::NONE);
    EXPECT_EQ(sprite->assetId, DEFAULT_NONE_BUTTON);
}

TEST(ButtonSystemCoverage, SkipsMissingComponents) {
    gameEngine::GameEngine engine;
    ButtonSystem system(engine, false);

    auto entity = engine.createEntity("Button");
    engine.addComponent<ButtonComponent>(entity, ButtonComponent({DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON}, []() {}));

    engine.setMousePosition(sf::Vector2i(10, 10));

    system.onUpdate(0.016f);

    auto& button = engine.getComponentEntity<ButtonComponent>(entity);
    EXPECT_EQ(button->state, ButtonState::NONE);
}
