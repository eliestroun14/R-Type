#include <gtest/gtest.h>

#define private public
#include <engine/render/RenderManager.hpp>
#undef private

TEST(RenderManagerCoverage, DefaultState)
{
    RenderManager manager;

    manager._mousePos = sf::Vector2i(0, 0);

    EXPECT_FALSE(manager.isOpen());
    EXPECT_FALSE(manager.isActionActive(GameAction::SHOOT));
    EXPECT_EQ(manager.getMousePosition(), sf::Vector2i(0, 0));
    EXPECT_EQ(manager.getScaleFactor(), 0.0f);
    EXPECT_EQ(manager.getTexture(Assets::LOGO_RTYPE), nullptr);
    EXPECT_EQ(manager.getFont(FontAssets::DEFAULT_FONT), nullptr);
}

TEST(RenderManagerCoverage, HandleKeyEventsAndJustPressed)
{
    RenderManager manager;

    manager.updateKeyBindings({{sf::Keyboard::Space, GameAction::SHOOT}});

    sf::Event pressed{};
    pressed.type = sf::Event::KeyPressed;
    pressed.key.code = sf::Keyboard::Space;
    manager.handleEvent(pressed);

    EXPECT_TRUE(manager.isActionActive(GameAction::SHOOT));

    manager._previousActions[GameAction::SHOOT] = false;
    EXPECT_TRUE(manager.isActionJustPressed(GameAction::SHOOT));

    sf::Event released{};
    released.type = sf::Event::KeyReleased;
    released.key.code = sf::Keyboard::Space;
    manager.handleEvent(released);

    manager._previousActions[GameAction::SHOOT] = true;
    EXPECT_TRUE(manager.isActionJustReleased(GameAction::SHOOT));
}

TEST(RenderManagerCoverage, HandleMouseEvents)
{
    RenderManager manager;

    sf::Event mouseLeft{};
    mouseLeft.type = sf::Event::MouseButtonPressed;
    mouseLeft.mouseButton.button = sf::Mouse::Left;
    manager.handleEvent(mouseLeft);
    EXPECT_TRUE(manager.isActionActive(GameAction::LEFT_CLICK));

    sf::Event mouseRight{};
    mouseRight.type = sf::Event::MouseButtonPressed;
    mouseRight.mouseButton.button = sf::Mouse::Right;
    manager.handleEvent(mouseRight);
    EXPECT_TRUE(manager.isActionActive(GameAction::RIGHT_CLICK));

    sf::Event mouseLeftRelease{};
    mouseLeftRelease.type = sf::Event::MouseButtonReleased;
    mouseLeftRelease.mouseButton.button = sf::Mouse::Left;
    manager.handleEvent(mouseLeftRelease);
    EXPECT_FALSE(manager.isActionActive(GameAction::LEFT_CLICK));
}
