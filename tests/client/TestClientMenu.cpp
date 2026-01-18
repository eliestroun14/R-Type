#include <gtest/gtest.h>

#define private public
#include <client/menu/ClientMenu.hpp>
#undef private

#include <engine/render/RenderManager.hpp>

#include "ClientUtilsTestStubs.hpp"

namespace {

std::shared_ptr<gameEngine::GameEngine> createTestEngine()
{
    auto engine = std::make_shared<gameEngine::GameEngine>();
    engine->init();

    engine->registerComponent<Transform>();
    engine->registerComponent<Sprite>();
    engine->registerComponent<Text>();
    engine->registerComponent<ButtonComponent>();
    engine->registerComponent<ScrollingBackground>();
    engine->registerComponent<GameConfig>();

    Entity configEntity = engine->createEntity("GameConfig");
    engine->addComponent(configEntity, GameConfig(FontAssets::DEFAULT_FONT, true, true));

    engine->_renderManager = std::make_shared<RenderManager>();

    return engine;
}

} // namespace

class ClientMenuTest : public ::testing::Test {
protected:
    std::shared_ptr<gameEngine::GameEngine> engine;
    std::unique_ptr<ClientMenu> menu;

    void SetUp() override
    {
        testutils::resetClientUtilsStubs();
        engine = createTestEngine();
        menu = std::make_unique<ClientMenu>(engine);
    }
};

TEST_F(ClientMenuTest, CreateMainMenuAndRunPendingActions)
{
    menu->createMainMenu();

    const auto* play = testutils::findButton("PLAY");
    const auto* options = testutils::findButton("OPTIONS");
    const auto* exitBtn = testutils::findButton("EXIT");

    ASSERT_NE(play, nullptr);
    ASSERT_NE(options, nullptr);
    ASSERT_NE(exitBtn, nullptr);

    play->onClick();
    options->onClick();
    exitBtn->onClick();

    menu->update();

    EXPECT_NE(testutils::findButton("back"), nullptr);
    EXPECT_NE(testutils::findButton("mOn"), nullptr);
}

TEST_F(ClientMenuTest, CreateOptionMenuTogglesAudioButtons)
{
    menu->createOptionMenu();

    const auto* musicOn = testutils::findButton("mOn");
    const auto* soundOn = testutils::findButton("sOn");

    ASSERT_NE(musicOn, nullptr);
    ASSERT_NE(soundOn, nullptr);

    musicOn->onClick();
    soundOn->onClick();

    menu->update();

    EXPECT_NE(testutils::findButton("mOff"), nullptr);
    EXPECT_NE(testutils::findButton("sOff"), nullptr);
}

TEST_F(ClientMenuTest, OptionMenuBackgroundOnlyCreatedOnce)
{
    menu->createOptionMenu();
    menu->createOptionMenu();

    EXPECT_EQ(testutils::movingBackgroundCount, 1);
}

TEST_F(ClientMenuTest, CreateKeybindsMenuAddsRebindButtons)
{
    menu->createKeybindsMenu();

    EXPECT_NE(testutils::findButton("UP"), nullptr);
    EXPECT_NE(testutils::findButton("DOWN"), nullptr);
    EXPECT_NE(testutils::findButton("LEFT"), nullptr);
    EXPECT_NE(testutils::findButton("RIGHT"), nullptr);
    EXPECT_NE(testutils::findButton("S"), nullptr);
    EXPECT_NE(testutils::findButton("D"), nullptr);
    EXPECT_NE(testutils::findButton("SPACE"), nullptr);
    EXPECT_NE(testutils::findButton("F"), nullptr);
}

TEST_F(ClientMenuTest, AccessibilityMenuTogglesFont)
{
    menu->createAccessibilityMenu();

    const auto* changeFont = testutils::findButton("CHANGE FONT");
    ASSERT_NE(changeFont, nullptr);
    changeFont->onClick();

    menu->update();

    auto& configs = engine->getComponents<GameConfig>();
    bool found = false;
    for (auto& config : configs) {
        if (config.has_value()) {
            EXPECT_EQ(config->activeFont, FontAssets::DYSLEXIC_FONT);
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(ClientMenuTest, CreatePlayMenuAddsBackButton)
{
    menu->createPlayMenu();

    EXPECT_NE(testutils::findButton("back"), nullptr);
}

TEST_F(ClientMenuTest, ClearMenuEntitiesKeepsBackground)
{
    menu->createMainMenu();
    EXPECT_FALSE(menu->_menuEntities.empty());

    menu->clearMenuEntities(true);

    EXPECT_FALSE(menu->_menuEntities.empty());

    bool hasBackground = false;
    for (const auto& entity : menu->_menuEntities) {
        if (engine->hasComponent<ScrollingBackground>(entity)) {
            hasBackground = true;
            break;
        }
    }
    EXPECT_TRUE(hasBackground);
}

TEST_F(ClientMenuTest, ClearMenuEntitiesResetsBackground)
{
    menu->createMainMenu();
    menu->clearMenuEntities(false);

    EXPECT_TRUE(menu->_menuEntities.empty());
    EXPECT_EQ(menu->_currentBackgroundAsset, Assets::MAIN_MENU_BG);
}

TEST(ClientMenuStandaloneTest, ClearMenuEntitiesWithNullEngine)
{
    ClientMenu menu(nullptr);
    menu.clearMenuEntities();
}
