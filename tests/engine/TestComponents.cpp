#include <gtest/gtest.h>

#include <engine/ecs/component/Components.hpp>
#include <common/protocol/Protocol.hpp>

TEST(ComponentsCoverage, ConstructCoreComponents)
{
    Transform transform(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(transform.x, 1.0f);
    EXPECT_FLOAT_EQ(transform.rotation, 3.0f);

    Velocity velocity(5.0f, 6.0f);
    EXPECT_FLOAT_EQ(velocity.vx, 5.0f);

    NetworkId netId(99, true);
    EXPECT_EQ(netId.id, 99u);
    EXPECT_TRUE(netId.isLocal);

    Sprite spriteWithRect(Assets::LOGO_RTYPE, ZIndex::IS_UI_HUD, sf::IntRect(1, 2, 3, 4));
    EXPECT_EQ(spriteWithRect.assetId, Assets::LOGO_RTYPE);

    Sprite spriteSimple(Assets::LOGO_RTYPE, ZIndex::IS_UI_HUD);
    EXPECT_EQ(spriteSimple.zIndex, ZIndex::IS_UI_HUD);

    Animation animation(32, 32, 1, 0.1f, 0.2f, 0, 3, true);
    EXPECT_EQ(animation.frameWidth, 32);
    EXPECT_TRUE(animation.loop);

    Text text("hello", sf::Color::White, 20, ZIndex::IS_UI_HUD);
    EXPECT_EQ(text.size, 20u);
    EXPECT_EQ(std::string(text.str).substr(0, 5), std::string("hello"));

    ScrollingBackground background(10.0f, true, false);
    EXPECT_FLOAT_EQ(background.scrollSpeed, 10.0f);
}

TEST(ComponentsCoverage, LogicAndGameplayComponents)
{
    VisualEffect effect(protocol::VisualEffectType::VFX_IMPACT_SPARK, 1.0f, 2.0f, 0.1f, 0.2f, 0.3f);
    EXPECT_EQ(effect.type, protocol::VisualEffectType::VFX_IMPACT_SPARK);

    Lifetime lifetime(5.0f);
    EXPECT_FLOAT_EQ(lifetime.remainingTime, 5.0f);

    Health health(50, 100);
    EXPECT_EQ(health.currentHealth, 50);
    EXPECT_EQ(health.maxHp, 100);

    Powerup powerup(PowerupType::HEAL, 3.5f);
    EXPECT_EQ(powerup.powerupType, PowerupType::HEAL);

    Weapon weapon(100, 10, 5, ProjectileType::LASER);
    EXPECT_EQ(weapon.fireRateMs, 100u);

    Clickable clickable(true);
    EXPECT_TRUE(clickable.isClicked);

    InputComponent input(7);
    EXPECT_EQ(input.playerId, 7u);

    Projectile projectile(Entity::fromId(3), true, 12);
    EXPECT_TRUE(projectile.isFromPlayable);

    MovementPattern pattern(MovementPatternType::SINE_WAVE, 2.0f, 1.0f, {{1.0f, 2.0f}}, 0.5f);
    EXPECT_EQ(pattern.patternType, MovementPatternType::SINE_WAVE);

    AI ai(AiBehaviour::ZIGZAG, 10.0f, 20.0f);
    EXPECT_EQ(ai.aiBehaviour, AiBehaviour::ZIGZAG);

    Force force(1, ForceAttachmentPoint::FORCE_FRONT, 2, 50, true);
    EXPECT_EQ(force.powerLevel, 2u);

    GameConfig config(FontAssets::DEFAULT_FONT, true, false);
    EXPECT_EQ(config.activeFont, FontAssets::DEFAULT_FONT);

    Rebind rebind(GameAction::MOVE_UP, Entity::fromId(1));
    EXPECT_EQ(rebind.action, GameAction::MOVE_UP);

    AudioEffect audioEffect(protocol::AudioEffectType::SFX_SHOOT_BASIC, 0.5f, 1.2f);
    EXPECT_FALSE(audioEffect.isPlaying);

    AudioSource audioSource(AudioAssets::SFX_SHOOT_BASIC, true, 50.0f, 0.2f, true, 0.3f);
    EXPECT_TRUE(audioSource.loop);
}

TEST(ComponentsCoverage, LevelDefaults)
{
    Level level;
    EXPECT_FALSE(level.started);
    EXPECT_EQ(level.currentWaveIndex, 0);
}

TEST(ComponentsCoverage, ButtonComponent)
{
    ButtonTextures textures{Assets::DEFAULT_NONE_BUTTON, Assets::DEFAULT_HOVER_BUTTON, Assets::DEFAULT_CLICKED_BUTTON};
    bool clicked = false;
    ButtonComponent button(textures, [&]() { clicked = true; });
    button.onClick();
    EXPECT_TRUE(clicked);
}
