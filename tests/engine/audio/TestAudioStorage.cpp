/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** AudioStorage tests
*/

#include <gtest/gtest.h>
#include <common/constants/render/Assets.hpp>
#include "engine/audio/AudioStorage.hpp"

class AudioStorageTest : public ::testing::Test {
protected:
    AudioStorage storage;
};

TEST_F(AudioStorageTest, GetSoundBufferReturnsNullWhenNotInitialized)
{
    auto buf = storage.getSoundBuffer(static_cast<AudioAssets>(0));
    EXPECT_EQ(buf, nullptr);
}

TEST_F(AudioStorageTest, InitLoadsOrThrowsResourceFailure)
{
    try {
        storage.init();
        for (int i = 0; i < static_cast<int>(NUMBER_AUDIO_ASSETS); ++i) {
            auto buf = storage.getSoundBuffer(static_cast<AudioAssets>(i));
            EXPECT_NE(buf, nullptr);
        }
    } catch (const Error& e) {
        EXPECT_EQ(e.getType(), ErrorType::ResourceLoadFailure);
    }
}

TEST_F(AudioStorageTest, GetSoundBufferReturnsNullOutOfRange)
{
    try {
        storage.init();
    } catch (const Error&) {
        // If assets missing, init throws; still proceed to out-of-range checks on empty storage
    }

    auto neg = storage.getSoundBuffer(static_cast<AudioAssets>(-1));
    auto tooHigh = storage.getSoundBuffer(static_cast<AudioAssets>(NUMBER_AUDIO_ASSETS));

    EXPECT_EQ(neg, nullptr);
    EXPECT_EQ(tooHigh, nullptr);
}
