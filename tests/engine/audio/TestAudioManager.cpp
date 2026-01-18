/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** AudioManager tests
*/

#include <gtest/gtest.h>
#include <SFML/Audio.hpp>

#define private public
#define protected public
#include "engine/audio/AudioManager.hpp"
#undef protected
#undef private

using namespace audio;

class AudioManagerTest : public ::testing::Test {
protected:
    AudioManager manager; // ctor runs and fills _activeSounds
};

TEST_F(AudioManagerTest, ConstructorInitializesSoundPool)
{
    EXPECT_EQ(manager._activeSounds.size(), 32u);
    // listener position initialized to (0,0) by constructor
    EXPECT_FLOAT_EQ(manager._listenerPosition.x, 0.0f);
    EXPECT_FLOAT_EQ(manager._listenerPosition.y, 0.0f);
}

TEST_F(AudioManagerTest, PlaySoundGracefullyHandlesMissingBuffer)
{
    // _audioStorage has no buffers → getSoundBuffer returns nullptr → no crash
    manager.playSound(protocol::AudioEffectType::SFX_SHOOT_BASIC, 10.f, 20.f, 0.5f, 1.2f);
    // The sound pool is still allocated
    EXPECT_EQ(manager._activeSounds.size(), 32u);
}

TEST_F(AudioManagerTest, PlaySoundUIGracefullyHandlesMissingBuffer)
{
    manager.playSoundUI(protocol::AudioEffectType::SFX_MENU_SELECT, 0.8f, 0.9f);
    EXPECT_EQ(manager._activeSounds.size(), 32u);
}

TEST_F(AudioManagerTest, PlayMusicFailsOnInvalidPathWithoutCrash)
{
    manager.playMusic("this/path/does/not/exist.ogg", 0.7f);
    // _music should exist after the call because it is allocated before openFromFile
    EXPECT_NE(manager._music, nullptr);
}

TEST_F(AudioManagerTest, StopMusicWorksWhenNoMusicAllocated)
{
    AudioManager localManager;
    ASSERT_EQ(localManager._music, nullptr);
    EXPECT_NO_THROW(localManager.stopMusic());
}

TEST_F(AudioManagerTest, StopMusicStopsWhenAllocated)
{
    manager._music = std::make_unique<sf::Music>();
    EXPECT_NO_THROW(manager.stopMusic());
}

TEST_F(AudioManagerTest, VolumeSettersClampValues)
{
    manager.setMasterVolume(2.0f);
    EXPECT_FLOAT_EQ(manager._masterVolume, 1.0f);

    manager.setSoundVolume(-1.0f);
    EXPECT_FLOAT_EQ(manager._soundVolume, 0.0f);

    manager.setMusicVolume(2.0f);
    EXPECT_FLOAT_EQ(manager._musicVolume, 1.0f);
}

TEST_F(AudioManagerTest, SetListenerPositionUpdatesVector)
{
    manager.setListenerPosition(42.0f, -13.0f);
    EXPECT_FLOAT_EQ(manager._listenerPosition.x, 42.0f);
    EXPECT_FLOAT_EQ(manager._listenerPosition.y, -13.0f);
}

TEST_F(AudioManagerTest, GetAvailableSoundReturnsSlot)
{
    sf::Sound* sound = manager.getAvailableSound();
    ASSERT_NE(sound, nullptr);
    // No buffer attached, status should not be Playing
    EXPECT_NE(sound->getStatus(), sf::Sound::Playing);
}

TEST_F(AudioManagerTest, MapProtocolToAssetReturnsFallbackOnUnknown)
{
    AudioAssets asset = manager.mapProtocolToAsset(static_cast<protocol::AudioEffectType>(0xFF));
    EXPECT_EQ(asset, AudioAssets::SFX_SHOOT_BASIC);
}
