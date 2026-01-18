#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <SFML/Graphics.hpp>
#include <engine/ecs/component/Components.hpp>
#include <common/protocol/Protocol.hpp>
#include <SFML/Audio.hpp>
#include <engine/audio/AudioStorage.hpp>
#include <common/logger/Logger.hpp>

namespace audio {

class AudioManager {
public:
    AudioManager();
    ~AudioManager() = default;

    void init();

    void playSound(protocol::AudioEffectType type, float x, float y, float volume = 1.0f, float pitch = 1.0f);
    void playSoundUI(protocol::AudioEffectType type, float volume = 1.0f, float pitch = 1.0f);

    void playMusic(const std::string& filepath, float volume = 0.5f);
    void stopMusic();

    void setListenerPosition(float x, float y);

    void setMasterVolume(float volume);
    void setSoundVolume(float volume);
    void setMusicVolume(float volume);

    void update();

    std::shared_ptr<sf::SoundBuffer> getSoundBuffer(AudioAssets id) const;

private:
    AudioStorage _audioStorage;

    std::vector<std::unique_ptr<sf::Sound>> _activeSounds;

    std::unique_ptr<sf::Music> _music;

    float _masterVolume = 1.0f;
    float _soundVolume = 1.0f;
    float _musicVolume = 0.5f;

    sf::Vector2f _listenerPosition;

    sf::Sound* getAvailableSound();

    AudioAssets mapProtocolToAsset(protocol::AudioEffectType type) const;
};

} // namespace audio
