/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** TestRenderManager
*/

#include <gtest/gtest.h>
#include <SFML/Config.hpp>
#include "../../../../../src/engine/include/engine/gameEngine/coordinator/render/RenderManager.hpp"

// Test inputs
TEST(RenderManagerTest, ProcessInputShooting) {
    RenderManager manager;

    EXPECT_FALSE(manager.isActionActive(GameAction::SHOOT));

    // fake event, press S
    sf::Event fakeEvent;
    fakeEvent.type = sf::Event::KeyPressed;
    fakeEvent.key.code = sf::Keyboard::S;

    manager.handleEvent(fakeEvent);

    // check result
    EXPECT_TRUE(manager.isActionActive(GameAction::SHOOT));
}

// Test released input
TEST(RenderManagerTest, ProcessInputRelease) {
    RenderManager manager;

    // simulate the pressure
    sf::Event pressEvent;
    pressEvent.type = sf::Event::KeyPressed;
    pressEvent.key.code = sf::Keyboard::Left;
    manager.handleEvent(pressEvent);
    
    EXPECT_TRUE(manager.isActionActive(GameAction::MOVE_LEFT));

    // simulate the release
    sf::Event releaseEvent;
    releaseEvent.type = sf::Event::KeyReleased;
    releaseEvent.key.code = sf::Keyboard::Left;
    manager.handleEvent(releaseEvent);

    EXPECT_FALSE(manager.isActionActive(GameAction::MOVE_LEFT));
}
