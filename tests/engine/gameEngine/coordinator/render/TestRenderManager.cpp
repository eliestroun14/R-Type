/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** TestRenderManager
*/

#include <gtest/gtest.h>
#include <SFML/Config.hpp>
#include <engine/render/RenderManager.hpp>

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

TEST(RenderManagerTest, ProcessInputUnboundKey) {
    RenderManager manager;

    // simulate user press 'A' (not set in the code)
    sf::Event unknownEvent;
    unknownEvent.type = sf::Event::KeyPressed;
    unknownEvent.key.code = sf::Keyboard::A;

    manager.handleEvent(unknownEvent);

    // check no action has been launched by accident
    EXPECT_FALSE(manager.isActionActive(GameAction::SHOOT));
    EXPECT_FALSE(manager.isActionActive(GameAction::MOVE_UP));

    // check it with getActiveActions
    auto actions = manager.getActiveActions();
    for (const auto& action : actions) {
        EXPECT_FALSE(action.second); // everything need to be false
    }
}

TEST(RenderManagerTest, ProcessInputMultipleKeys) {
    RenderManager manager;

    // press UP
    sf::Event moveEvent;
    moveEvent.type = sf::Event::KeyPressed;
    moveEvent.key.code = sf::Keyboard::Up;
    manager.handleEvent(moveEvent);

    // press 'S' (to shoot)
    sf::Event shootEvent;
    shootEvent.type = sf::Event::KeyPressed;
    shootEvent.key.code = sf::Keyboard::S;
    manager.handleEvent(shootEvent);

    // check 2 actions are activated
    EXPECT_TRUE(manager.isActionActive(GameAction::MOVE_UP));
    EXPECT_TRUE(manager.isActionActive(GameAction::SHOOT));
}

TEST(RenderManagerTest, InitialStateIsClean) {
    RenderManager manager;

    // the constructor only bind the keys viable, and init() set the window so the window must be closed here
    EXPECT_FALSE(manager.isOpen());

    // check every actions are false
    EXPECT_FALSE(manager.isActionActive(GameAction::EXIT));
    EXPECT_FALSE(manager.isActionActive(GameAction::SPECIAL));

    // test with an action that does not exit in the map
    EXPECT_FALSE(manager.isActionActive(GameAction::UNKNOW));
}

TEST(RenderManagerTest, HandleCloseEvent) {
    RenderManager manager;

    // simulation of the close event for the window (without a window lol)
    sf::Event closeEvent;
    closeEvent.type = sf::Event::Closed;

    EXPECT_NO_THROW(manager.handleEvent(closeEvent));
}
