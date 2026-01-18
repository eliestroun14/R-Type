#include <gtest/gtest.h>

#include <game/utils/ClientUtils.hpp>
#include <engine/GameEngine.hpp>
#include <engine/ecs/component/Components.hpp>
#include <common/constants/render/Assets.hpp>

namespace {

gameEngine::GameEngine makeEngine()
{
    gameEngine::GameEngine engine;
    engine.init();
    engine.registerComponent<Transform>();
    engine.registerComponent<Sprite>();
    engine.registerComponent<ButtonComponent>();
    engine.registerComponent<Text>();
    engine.registerComponent<Animation>();
    engine.registerComponent<ScrollingBackground>();
    engine.registerComponent<Rebind>();
    engine.registerComponent<GameConfig>();
    return engine;
}

} // namespace

TEST(ClientUtilsCoverage, GenerateClientIdAndTime)
{
    auto id1 = generateClientId();
    auto id2 = generateClientId();
    EXPECT_NE(id1, id2);

    auto t1 = getCurrentTimeMs();
    auto t2 = getCurrentTimeMs();
    EXPECT_LE(t1, t2);
}

TEST(ClientUtilsCoverage, CreateButtonCreatesTwoEntities)
{
    auto engine = makeEngine();

    auto entities = createButton(engine,
        "Play",
        20,
        sf::Color::White,
        sf::Vector2f(10.0f, 20.0f),
        1.5f,
        sf::IntRect(0, 0, 100, 40),
        DEFAULT_NONE_BUTTON,
        DEFAULT_HOVER_BUTTON,
        DEFAULT_CLICKED_BUTTON,
        []() {});

    ASSERT_EQ(entities.size(), 2u);

    Entity button = entities[0];
    Entity text = entities[1];

    EXPECT_TRUE(engine.hasComponent<Transform>(button));
    EXPECT_TRUE(engine.hasComponent<Sprite>(button));
    EXPECT_TRUE(engine.hasComponent<ButtonComponent>(button));

    EXPECT_TRUE(engine.hasComponent<Transform>(text));
    EXPECT_TRUE(engine.hasComponent<Text>(text));
}

TEST(ClientUtilsCoverage, CreateTextImageAndAnimatedImage)
{
    auto engine = makeEngine();

    Entity text = createText(engine, "Hello", 18, sf::Color::White, sf::Vector2f(5.0f, 6.0f), 0.0f, 1.0f);
    EXPECT_TRUE(engine.hasComponent<Text>(text));

    Entity image = createImage(engine, DEFAULT_BULLET, sf::Vector2f(3.0f, 4.0f), 0.0f, 1.0f,
        sf::IntRect(0, 0, 16, 16), ZIndex::IS_GAME);
    EXPECT_TRUE(engine.hasComponent<Sprite>(image));

    Animation anim(16, 16, 0, 0.0f, 0.1f, 0, 3, true);
    Entity animImage = createAnimatedImage(engine, DEFAULT_BULLET, anim, sf::Vector2f(1.0f, 2.0f), 0.0f, 1.0f,
        sf::IntRect(0, 0, 16, 16), ZIndex::IS_GAME);
    EXPECT_TRUE(engine.hasComponent<Animation>(animImage));
}

TEST(ClientUtilsCoverage, CreateMovingBackground)
{
    auto engine = makeEngine();

    Entity bg = createMovingBackground(engine, STARS_BG, sf::Vector2f(0.0f, 0.0f), 0.0f, 1.0f,
        sf::IntRect(0, 0, 100, 50), 5.0f, true, true);

    EXPECT_TRUE(engine.hasComponent<ScrollingBackground>(bg));
    auto& sb = engine.getComponentEntity<ScrollingBackground>(bg).value();
    EXPECT_TRUE(sb.repeat);
    EXPECT_TRUE(sb.horizontal);
}

TEST(ClientUtilsCoverage, CreateRebindButtonAndOnClick)
{
    auto engine = makeEngine();

    auto entities = createRebindButton(engine, "Shoot", GameAction::SHOOT, 16, sf::Vector2f(10.0f, 10.0f), 1.0f);
    ASSERT_EQ(entities.size(), 2u);

    Entity button = entities[0];
    Entity text = entities[1];

    EXPECT_TRUE(engine.hasComponent<Rebind>(button));
    EXPECT_TRUE(engine.hasComponent<ButtonComponent>(button));
    EXPECT_TRUE(engine.hasComponent<Text>(text));

    auto& btn = engine.getComponentEntity<ButtonComponent>(button).value();
    ASSERT_TRUE(static_cast<bool>(btn.onClick));

    btn.onClick();

    auto& rebind = engine.getComponentEntity<Rebind>(button).value();
    auto& txt = engine.getComponentEntity<Text>(text).value();

    EXPECT_TRUE(rebind.isWaiting);
    EXPECT_STREQ(txt.str, "...");
}

TEST(ClientUtilsCoverage, KeyToStringAndConfigLookup)
{
    EXPECT_EQ(keyToString(sf::Keyboard::A), "A");
    EXPECT_EQ(keyToString(sf::Keyboard::Num3), "3");
    EXPECT_EQ(keyToString(sf::Keyboard::Space), "SPACE");
    EXPECT_EQ(keyToString(sf::Keyboard::Unknown), "UNKNOWN");

    auto engine = makeEngine();
    Entity config = engine.createEntity("config");
    engine.addComponent(config, GameConfig(FontAssets::DEFAULT_FONT, true, true));

    EXPECT_EQ(getKeybordKeyFromGameConfig(engine, GameAction::MOVE_LEFT), "LEFT");
    EXPECT_EQ(getKeybordKeyFromGameConfig(engine, GameAction::SHOOT), "SPACE");
}
