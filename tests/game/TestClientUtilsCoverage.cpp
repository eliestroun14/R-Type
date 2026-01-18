/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ClientUtils coverage tests
*/

#include <gtest/gtest.h>

#include <limits>
#include <string>

#include <game/utils/ClientUtils.hpp>

TEST(ClientUtilsCoverage, GenerateClientIdReturnsValue) {
    auto id = generateClientId();
    EXPECT_LE(id, std::numeric_limits<uint32_t>::max());
}

TEST(ClientUtilsCoverage, GetCurrentTimeMsIsMonotonic) {
    auto t1 = getCurrentTimeMs();
    auto t2 = getCurrentTimeMs();
    EXPECT_GE(t2, t1);
}

TEST(ClientUtilsCoverage, CreateButtonCreatesEntitiesAndComponents) {
    gameEngine::GameEngine engine;
    auto entities = createButton(
        engine,
        "Play",
        24,
        sf::Color::White,
        sf::Vector2f(10.0f, 20.0f),
        2.0f,
        sf::IntRect(0, 0, 100, 40),
        DEFAULT_NONE_BUTTON,
        DEFAULT_HOVER_BUTTON,
        DEFAULT_CLICKED_BUTTON,
        nullptr
    );

    ASSERT_EQ(entities.size(), 2u);

    auto button = entities[0];
    auto text = entities[1];

    auto& buttonTransform = engine.getComponentEntity<Transform>(button);
    auto& buttonSprite = engine.getComponentEntity<Sprite>(button);
    auto& buttonComp = engine.getComponentEntity<ButtonComponent>(button);

    ASSERT_TRUE(buttonTransform.has_value());
    ASSERT_TRUE(buttonSprite.has_value());
    ASSERT_TRUE(buttonComp.has_value());

    EXPECT_FLOAT_EQ(buttonTransform->x, 10.0f);
    EXPECT_FLOAT_EQ(buttonTransform->y, 20.0f);
    EXPECT_FLOAT_EQ(buttonTransform->scale, 2.0f);
    EXPECT_EQ(buttonSprite->assetId, DEFAULT_NONE_BUTTON);
    EXPECT_EQ(buttonSprite->zIndex, ZIndex::IS_GAME);
    EXPECT_EQ(buttonComp->textures.none, DEFAULT_NONE_BUTTON);
    EXPECT_EQ(buttonComp->textures.hover, DEFAULT_HOVER_BUTTON);
    EXPECT_EQ(buttonComp->textures.clicked, DEFAULT_CLICKED_BUTTON);

    auto& textTransform = engine.getComponentEntity<Transform>(text);
    auto& textComp = engine.getComponentEntity<Text>(text);

    ASSERT_TRUE(textTransform.has_value());
    ASSERT_TRUE(textComp.has_value());

    EXPECT_FLOAT_EQ(textTransform->x, 10.0f + (100.0f * 2.0f) / 2.0f);
    EXPECT_FLOAT_EQ(textTransform->y, 20.0f + (40.0f * 2.0f) / 2.0f);
    EXPECT_EQ(std::string(textComp->str), std::string("Play"));
}

TEST(ClientUtilsCoverage, CreateTextCreatesTextEntity) {
    gameEngine::GameEngine engine;
    auto entity = createText(
        engine,
        "Title",
        32,
        sf::Color::White,
        sf::Vector2f(100.0f, 50.0f),
        15.0f,
        1.5f
    );

    auto& transform = engine.getComponentEntity<Transform>(entity);
    auto& text = engine.getComponentEntity<Text>(entity);

    ASSERT_TRUE(transform.has_value());
    ASSERT_TRUE(text.has_value());
    EXPECT_FLOAT_EQ(transform->x, 100.0f);
    EXPECT_FLOAT_EQ(transform->y, 50.0f);
    EXPECT_FLOAT_EQ(transform->rotation, 15.0f);
    EXPECT_FLOAT_EQ(transform->scale, 1.5f);
    EXPECT_EQ(std::string(text->str), std::string("Title"));
}

TEST(ClientUtilsCoverage, CreateImageCreatesSpriteEntity) {
    gameEngine::GameEngine engine;
    auto entity = createImage(
        engine,
        DEFAULT_NONE_BUTTON,
        sf::Vector2f(5.0f, 6.0f),
        0.0f,
        1.0f,
        sf::IntRect(0, 0, 12, 34),
        ZIndex::IS_UI_HUD
    );

    auto& transform = engine.getComponentEntity<Transform>(entity);
    auto& sprite = engine.getComponentEntity<Sprite>(entity);

    ASSERT_TRUE(transform.has_value());
    ASSERT_TRUE(sprite.has_value());
    EXPECT_FLOAT_EQ(transform->x, 5.0f);
    EXPECT_FLOAT_EQ(transform->y, 6.0f);
    EXPECT_EQ(sprite->assetId, DEFAULT_NONE_BUTTON);
    EXPECT_EQ(sprite->zIndex, ZIndex::IS_UI_HUD);
    EXPECT_EQ(sprite->rect.width, 12);
    EXPECT_EQ(sprite->rect.height, 34);
}

TEST(ClientUtilsCoverage, CreateAnimatedImageCreatesAnimationComponent) {
    gameEngine::GameEngine engine;
    Animation anim(32, 32, 0, 0.0f, 0.1f, 0, 3, true);

    auto entity = createAnimatedImage(
        engine,
        DEFAULT_NONE_BUTTON,
        anim,
        sf::Vector2f(1.0f, 2.0f),
        0.0f,
        1.0f,
        sf::IntRect(0, 0, 32, 32),
        ZIndex::IS_GAME
    );

    auto& sprite = engine.getComponentEntity<Sprite>(entity);
    auto& animation = engine.getComponentEntity<Animation>(entity);

    ASSERT_TRUE(sprite.has_value());
    ASSERT_TRUE(animation.has_value());
    EXPECT_EQ(sprite->assetId, DEFAULT_NONE_BUTTON);
    EXPECT_EQ(animation->frameWidth, 32);
    EXPECT_EQ(animation->endFrame, 3);
}

TEST(ClientUtilsCoverage, CreateMovingBackgroundCreatesScrollingComponent) {
    gameEngine::GameEngine engine;
    auto entity = createMovingBackground(
        engine,
        DEFAULT_NONE_BUTTON,
        sf::Vector2f(0.0f, 0.0f),
        0.0f,
        1.0f,
        sf::IntRect(0, 0, 200, 100),
        42.0f,
        true,
        false
    );

    auto& scroll = engine.getComponentEntity<ScrollingBackground>(entity);
    ASSERT_TRUE(scroll.has_value());
    EXPECT_FLOAT_EQ(scroll->scrollSpeed, 42.0f);
    EXPECT_TRUE(scroll->horizontal);
    EXPECT_FALSE(scroll->repeat);
}

TEST(ClientUtilsCoverage, CreateRebindButtonSetsUpRebindAndOnClick) {
    gameEngine::GameEngine engine;
    auto entities = createRebindButton(
        engine,
        "Rebind",
        GameAction::SHOOT,
        18,
        sf::Vector2f(0.0f, 0.0f),
        1.0f
    );

    ASSERT_EQ(entities.size(), 2u);
    auto button = entities[0];
    auto text = entities[1];

    auto& rebind = engine.getComponentEntity<Rebind>(button);
    auto& buttonComp = engine.getComponentEntity<ButtonComponent>(button);
    auto& textComp = engine.getComponentEntity<Text>(text);

    ASSERT_TRUE(rebind.has_value());
    ASSERT_TRUE(buttonComp.has_value());
    ASSERT_TRUE(textComp.has_value());

    EXPECT_EQ(rebind->action, GameAction::SHOOT);
    EXPECT_FALSE(rebind->isWaiting);

    ASSERT_TRUE(static_cast<bool>(buttonComp->onClick));
    buttonComp->onClick();

    EXPECT_TRUE(rebind->isWaiting);
    EXPECT_EQ(std::string(textComp->str), std::string("..."));
}
