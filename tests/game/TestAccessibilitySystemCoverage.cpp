/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** AccessibilitySystem coverage tests
*/

#include <gtest/gtest.h>

#define private public
#define protected public
#include <game/systems/AccessibilitySystem.hpp>
#undef protected
#undef private

TEST(AccessibilitySystemCoverage, UpdateSkipsWhenNoConfig) {
    gameEngine::GameEngine engine;
    AccessibilitySystem system(engine);

    // No GameConfig components added
    system.onUpdate(0.016f);
    SUCCEED();
}

TEST(AccessibilitySystemCoverage, UpdateReadsFirstConfig) {
    gameEngine::GameEngine engine;
    AccessibilitySystem system(engine);

    auto entity = engine.createEntity("Config");
    engine.addComponent<GameConfig>(entity, GameConfig(FontAssets::DEFAULT_FONT, true, true));

    system.onUpdate(0.016f);
    SUCCEED();
}
