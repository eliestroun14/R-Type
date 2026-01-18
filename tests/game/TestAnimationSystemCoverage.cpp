/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** AnimationSystem coverage tests
*/

#include <gtest/gtest.h>

#include <vector>

#define private public
#define protected public
#include <game/systems/AnimationSystem.hpp>
#undef protected
#undef private

TEST(AnimationSystemCoverage, UpdatesFrameAndSpriteRectWhenElapsedExceedsDuration) {
    gameEngine::GameEngine engine;
    AnimationSystem system(engine);

    auto entity = system._engine.createEntity("AnimEntity");
    system._engine.addComponent<Sprite>(entity, Sprite(DEFAULT_NONE_BUTTON, ZIndex::IS_GAME, sf::IntRect(0, 0, 16, 16)));
    system._engine.addComponent<Animation>(entity, Animation(16, 16, 0, 0.0f, 0.1f, 0, 3, true));

    system._entities = {static_cast<size_t>(entity)};

    system.onUpdate(0.2f);

    auto& anim = system._engine.getComponents<Animation>()[static_cast<size_t>(entity)].value();
    auto& sprite = system._engine.getComponents<Sprite>()[static_cast<size_t>(entity)].value();

    EXPECT_EQ(anim.currentFrame, 1);
    EXPECT_FLOAT_EQ(anim.elapsedTime, 0.0f);
    EXPECT_EQ(sprite.rect.left, 16);
}

TEST(AnimationSystemCoverage, ClampsFrameWhenNotLooping) {
    gameEngine::GameEngine engine;
    AnimationSystem system(engine);

    auto entity = system._engine.createEntity("AnimClamp");
    system._engine.addComponent<Sprite>(entity, Sprite(DEFAULT_NONE_BUTTON, ZIndex::IS_GAME, sf::IntRect(0, 0, 8, 8)));
    system._engine.addComponent<Animation>(entity, Animation(8, 8, 2, 0.0f, 0.1f, 0, 2, false));

    system._entities = {static_cast<size_t>(entity)};

    system.onUpdate(0.2f);

    auto& anim = system._engine.getComponents<Animation>()[static_cast<size_t>(entity)].value();
    EXPECT_EQ(anim.currentFrame, 2);
}

TEST(AnimationSystemCoverage, LoopsFrameWhenLooping) {
    gameEngine::GameEngine engine;
    AnimationSystem system(engine);

    auto entity = system._engine.createEntity("AnimLoop");
    system._engine.addComponent<Sprite>(entity, Sprite(DEFAULT_NONE_BUTTON, ZIndex::IS_GAME, sf::IntRect(0, 0, 8, 8)));
    system._engine.addComponent<Animation>(entity, Animation(8, 8, 2, 0.0f, 0.1f, 1, 2, true));

    system._entities = {static_cast<size_t>(entity)};

    system.onUpdate(0.2f);

    auto& anim = system._engine.getComponents<Animation>()[static_cast<size_t>(entity)].value();
    EXPECT_EQ(anim.currentFrame, 1);
}

TEST(AnimationSystemCoverage, SkipsEntitiesWithoutComponents) {
    gameEngine::GameEngine engine;
    AnimationSystem system(engine);

    auto entityWithAnimOnly = system._engine.createEntity("AnimOnly");
    system._engine.addComponent<Animation>(entityWithAnimOnly, Animation(8, 8, 0, 0.0f, 0.1f, 0, 1, true));

    auto entityWithSpriteOnly = system._engine.createEntity("SpriteOnly");
    system._engine.addComponent<Sprite>(entityWithSpriteOnly, Sprite(DEFAULT_NONE_BUTTON, ZIndex::IS_GAME, sf::IntRect(0, 0, 8, 8)));

    system._entities = {static_cast<size_t>(entityWithAnimOnly), static_cast<size_t>(entityWithSpriteOnly)};

    system.onUpdate(0.2f);

    auto& anim = system._engine.getComponents<Animation>()[static_cast<size_t>(entityWithAnimOnly)].value();
    EXPECT_EQ(anim.currentFrame, 0);
}
