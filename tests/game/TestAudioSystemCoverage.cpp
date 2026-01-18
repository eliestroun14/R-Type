/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** AudioSystem coverage tests
*/

#include <gtest/gtest.h>

#define private public
#define protected public
#include <game/systems/AudioSystem.hpp>
#undef protected
#undef private

TEST(AudioSystemCoverage, SkipsEntitiesMissingComponents) {
    gameEngine::GameEngine engine;
    AudioSystem system(engine);

    auto audioOnly = engine.createEntity("AudioOnly");
    engine.addComponent<AudioSource>(audioOnly, AudioSource(AudioAssets::SFX_SHOOT_BASIC, false));

    auto transformOnly = engine.createEntity("TransformOnly");
    engine.addComponent<Transform>(transformOnly, Transform(1.0f, 2.0f, 0.0f, 1.0f));

    system._entities = {static_cast<size_t>(audioOnly), static_cast<size_t>(transformOnly)};

    system.onUpdate(0.1f);

    EXPECT_EQ(engine.playSoundCalls, 0);
    EXPECT_EQ(engine.playSoundUICalls, 0);
}

TEST(AudioSystemCoverage, OneShotPlaysOnce) {
    gameEngine::GameEngine engine;
    AudioSystem system(engine);

    auto entity = engine.createEntity("OneShot");
    engine.addComponent<Transform>(entity, Transform(10.0f, 20.0f, 0.0f, 1.0f));
    engine.addComponent<AudioSource>(entity, AudioSource(AudioAssets::SFX_SHOOT_BASIC, false));

    system._entities = {static_cast<size_t>(entity)};

    system.onUpdate(0.1f);
    EXPECT_EQ(engine.playSoundCalls, 1);

    system.onUpdate(0.1f);
    EXPECT_EQ(engine.playSoundCalls, 1);
}

TEST(AudioSystemCoverage, LoopingReplaysAfterDuration) {
    gameEngine::GameEngine engine;
    AudioSystem system(engine);

    auto entity = engine.createEntity("Looping");
    engine.addComponent<Transform>(entity, Transform(5.0f, 6.0f, 0.0f, 1.0f));
    engine.addComponent<AudioSource>(entity, AudioSource(AudioAssets::SFX_SHOOT_CHARGED, true, 100.0f, 0.5f, false, 0.2f));

    system._entities = {static_cast<size_t>(entity)};

    system.onUpdate(0.1f);
    EXPECT_EQ(engine.playSoundCalls, 1);

    system.onUpdate(0.2f);
    EXPECT_EQ(engine.playSoundCalls, 2);
}

TEST(AudioSystemCoverage, PlaysUISoundWhenFlagSet) {
    gameEngine::GameEngine engine;
    AudioSystem system(engine);

    auto entity = engine.createEntity("UI");
    engine.addComponent<Transform>(entity, Transform(0.0f, 0.0f, 0.0f, 1.0f));
    engine.addComponent<AudioSource>(entity, AudioSource(AudioAssets::SFX_MENU_SELECT, false, 0.0f, 0.0f, true, 0.1f));

    system._entities = {static_cast<size_t>(entity)};

    system.onUpdate(0.1f);
    EXPECT_EQ(engine.playSoundUICalls, 1);
    EXPECT_EQ(engine.playSoundCalls, 0);
}

TEST(AudioSystemCoverage, MapsAudioAssetsToProtocol) {
    gameEngine::GameEngine engine;
    AudioSystem system(engine);

    EXPECT_EQ(system.getAudioEffectFromAsset(AudioAssets::SFX_SHOOT_BASIC),
              protocol::AudioEffectType::SFX_SHOOT_BASIC);
    EXPECT_EQ(system.getAudioEffectFromAsset(AudioAssets::SFX_EXPLOSION_LARGE_2),
              protocol::AudioEffectType::SFX_EXPLOSION_LARGE_2);
    EXPECT_EQ(system.getAudioEffectFromAsset(AudioAssets::MAIN_MENU_MUSIC),
              protocol::AudioEffectType::SFX_SHOOT_BASIC);
}
