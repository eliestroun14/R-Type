#include <gtest/gtest.h>

#define private public
#include <engine/GameEngine.hpp>
#undef private

#include <engine/audio/AudioManager.hpp>
#include <engine/ecs/component/Components.hpp>
#include <game/systems/AudioSystem.hpp>

namespace {

AudioSystem& setupAudioSystem(gameEngine::GameEngine& engine)
{
    engine.init();
    engine.registerComponent<AudioSource>();
    engine.registerComponent<Transform>();
    engine.registerComponent<GameConfig>();

    engine._audioManager = std::make_shared<audio::AudioManager>();

    auto& system = engine.registerSystem<AudioSystem>(engine);
    engine.setSystemSignature<AudioSystem, AudioSource>();
    return system;
}

Entity createAudioEntity(gameEngine::GameEngine& engine, const AudioSource& source)
{
    Entity entity = engine.createEntity("audio");
    engine.addComponent(entity, AudioSource(source));
    return entity;
}

} // namespace

TEST(AudioSystemCoverage, ConstructorAndOnCreate)
{
    gameEngine::GameEngine engine;
    engine.init();

    AudioSystem system(engine);
    EXPECT_FALSE(system.isRunning());

    system.onCreate();
    SUCCEED();
}

TEST(AudioSystemCoverage, ReturnsWhenAudioDisabled)
{
    gameEngine::GameEngine engine;
    auto& system = setupAudioSystem(engine);

    Entity config = engine.createEntity("config");
    engine.addComponent(config, GameConfig(FontAssets::DEFAULT_FONT, false, false));

    Entity audioEntity = createAudioEntity(engine, AudioSource(AudioAssets::SFX_MENU_SELECT, false, 0.0f, 0.0f, true, 0.1f));

    system.onUpdate(0.5f);

    auto& audio = engine.getComponents<AudioSource>()[static_cast<size_t>(audioEntity)].value();
    EXPECT_FALSE(audio.hasBeenPlayed);
    EXPECT_FLOAT_EQ(audio.elapsedTimeSincePlay, 0.0f);
}

TEST(AudioSystemCoverage, OneShotAndLoopingBehavior)
{
    gameEngine::GameEngine engine;
    auto& system = setupAudioSystem(engine);

    Entity config = engine.createEntity("config");
    engine.addComponent(config, GameConfig(FontAssets::DEFAULT_FONT, true, true));

    Entity oneShot = createAudioEntity(engine, AudioSource(AudioAssets::SFX_MENU_SELECT, false, 0.0f, 0.0f, true, 0.1f));

    system.onUpdate(0.05f);
    auto& oneShotAudio = engine.getComponents<AudioSource>()[static_cast<size_t>(oneShot)].value();
    EXPECT_TRUE(oneShotAudio.hasBeenPlayed);
    EXPECT_FLOAT_EQ(oneShotAudio.elapsedTimeSincePlay, 0.0f);

    system.onUpdate(0.05f);
    EXPECT_TRUE(oneShotAudio.hasBeenPlayed);

    Entity looping = createAudioEntity(engine, AudioSource(AudioAssets::SFX_MENU_SELECT, true, 0.0f, 0.0f, true, 0.1f));

    system.onUpdate(0.05f);
    auto& loopingAudio = engine.getComponents<AudioSource>()[static_cast<size_t>(looping)].value();
    EXPECT_TRUE(loopingAudio.hasBeenPlayed);
    EXPECT_FLOAT_EQ(loopingAudio.elapsedTimeSincePlay, 0.0f);

    system.onUpdate(0.05f);
    EXPECT_LT(loopingAudio.elapsedTimeSincePlay, loopingAudio.soundDuration);

    system.onUpdate(0.1f);
    EXPECT_FLOAT_EQ(loopingAudio.elapsedTimeSincePlay, 0.0f);
}

TEST(AudioSystemCoverage, Skips3DSoundWithoutTransform)
{
    gameEngine::GameEngine engine;
    auto& system = setupAudioSystem(engine);

    Entity config = engine.createEntity("config");
    engine.addComponent(config, GameConfig(FontAssets::DEFAULT_FONT, true, true));

    Entity audioEntity = createAudioEntity(engine, AudioSource(AudioAssets::SFX_SHOOT_BASIC, false, 0.0f, 0.0f, false, 0.1f));

    system.onUpdate(0.1f);

    auto& audio = engine.getComponents<AudioSource>()[static_cast<size_t>(audioEntity)].value();
    EXPECT_FALSE(audio.hasBeenPlayed);
    EXPECT_FLOAT_EQ(audio.elapsedTimeSincePlay, 0.0f);
}

TEST(AudioSystemCoverage, Plays3DSoundWhenTransformPresent)
{
    gameEngine::GameEngine engine;
    auto& system = setupAudioSystem(engine);

    Entity config = engine.createEntity("config");
    engine.addComponent(config, GameConfig(FontAssets::DEFAULT_FONT, true, true));

    Entity audioEntity = createAudioEntity(engine, AudioSource(AudioAssets::SFX_SHOOT_BASIC, false, 0.0f, 0.0f, false, 0.1f));
    engine.addComponent(audioEntity, Transform(1.0f, 2.0f, 0.0f, 1.0f));

    system.onUpdate(0.1f);

    auto& audio = engine.getComponents<AudioSource>()[static_cast<size_t>(audioEntity)].value();
    EXPECT_TRUE(audio.hasBeenPlayed);
    EXPECT_FLOAT_EQ(audio.elapsedTimeSincePlay, 0.0f);
}

TEST(AudioSystemCoverage, RespectsMusicAndSoundToggles)
{
    gameEngine::GameEngine engine;
    auto& system = setupAudioSystem(engine);

    Entity config = engine.createEntity("config");
    engine.addComponent(config, GameConfig(FontAssets::DEFAULT_FONT, false, true));

    Entity looping = createAudioEntity(engine, AudioSource(AudioAssets::MAIN_MENU_MUSIC, true, 0.0f, 0.0f, true, 0.1f));
    Entity oneShot = createAudioEntity(engine, AudioSource(AudioAssets::SFX_MENU_SELECT, false, 0.0f, 0.0f, true, 0.1f));

    system.onUpdate(0.1f);

    auto& loopingAudio = engine.getComponents<AudioSource>()[static_cast<size_t>(looping)].value();
    auto& oneShotAudio = engine.getComponents<AudioSource>()[static_cast<size_t>(oneShot)].value();

    EXPECT_FALSE(loopingAudio.hasBeenPlayed);
    EXPECT_TRUE(oneShotAudio.hasBeenPlayed);
}
