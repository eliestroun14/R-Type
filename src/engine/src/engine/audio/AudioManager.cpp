/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** AudioManager
*/

#include <engine/audio/AudioManager.hpp>
#include <iostream>

namespace audio {

AudioManager::AudioManager()
{
    sf::Listener::setGlobalVolume(100.0f);
    sf::Listener::setPosition(0.0f, 0.0f, 0.0f);
    sf::Listener::setDirection(0.0f, 0.0f, -1.0f);
    sf::Listener::setUpVector(0.0f, 1.0f, 0.0f);

    for (int i = 0; i < 32; ++i)
        this->_activeSounds.push_back(std::make_unique<sf::Sound>());
}

void AudioManager::init()
{
    this->_audioStorage.init();
    LOG_INFO("[AudioManager] All sounds loaded successfully");
}

void AudioManager::playSound(protocol::AudioEffectType type, float x, float y, float volume, float pitch)
{
    AudioAssets assetId = mapProtocolToAsset(type);
    auto buffer = this->_audioStorage.getSoundBuffer(assetId);

    if (!buffer) {
        int type = static_cast<int>(type);
        LOG_ERROR_CAT("[AudioManager] Sound buffer not found for type ", std::to_string(type));
        return;
    }

    sf::Sound* sound = getAvailableSound();
    if (!sound) {
        LOG_ERROR("[AudioManager] No available sound slot!");
        return;
    }

    sound->setBuffer(*buffer);
    sound->setVolume(volume * this->_soundVolume * this->_masterVolume * 100.0f);
    sound->setPitch(pitch);

    sound->setPosition(x, y, 0.0f);
    sound->setRelativeToListener(false);

    sound->setAttenuation(0.01f);
    sound->setMinDistance(50.0f);

    sound->play();
}

void AudioManager::playSoundUI(protocol::AudioEffectType type, float volume, float pitch)
{
    AudioAssets assetId = mapProtocolToAsset(type);
    auto buffer = this->_audioStorage.getSoundBuffer(assetId);

    if (!buffer) {
        int type = static_cast<int>(type);
        LOG_ERROR_CAT("[AudioManager] Sound buffer not found for type ", std::to_string(type));
        return;
    }

    sf::Sound* sound = getAvailableSound();
    if (!sound) {
        LOG_ERROR("[AudioManager] No available sound slot!");
        return;
    }

    sound->setBuffer(*buffer);
    sound->setVolume(volume * this->_soundVolume * this->_masterVolume * 100.0f);
    sound->setPitch(pitch);

    sound->setRelativeToListener(true);
    sound->setPosition(0.0f, 0.0f, 0.0f);

    sound->play();
}

void AudioManager::playMusic(const std::string& filepath, float volume)
{
    if (!this->_music)
        this->_music = std::make_unique<sf::Music>();

    if (!this->_music->openFromFile(filepath)) {
        LOG_ERROR_CAT("[AudioManager] Failed to load music: ", filepath);
        return;
    }

    this->_music->setVolume(volume * this->_musicVolume * this->_masterVolume * 100.0f);
    this->_music->setLoop(true);
    this->_music->play();
    LOG_INFO_CAT("[AudioManager] Music started: ", filepath);

}

void AudioManager::stopMusic()
{
    if (this->_music)
        this->_music->stop();
}

void AudioManager::setListenerPosition(float x, float y)
{
    this->_listenerPosition = sf::Vector2f(x, y);
    sf::Listener::setPosition(x, y, 0.0f);
}

void AudioManager::setMasterVolume(float volume)
{
    this->_masterVolume = std::clamp(volume, 0.0f, 1.0f);
}

void AudioManager::setSoundVolume(float volume)
{
    this->_soundVolume = std::clamp(volume, 0.0f, 1.0f);
}

void AudioManager::setMusicVolume(float volume)
{
    this->_musicVolume = std::clamp(volume, 0.0f, 1.0f);
    if (this->_music) {
        this->_music->setVolume(this->_musicVolume * this->_masterVolume * 100.0f);
    }
}

void AudioManager::update()
{
}

std::shared_ptr<sf::SoundBuffer> AudioManager::getSoundBuffer(AudioAssets id) const
{
    return this->_audioStorage.getSoundBuffer(id);
}

sf::Sound* AudioManager::getAvailableSound()
{
    // find sound not playing
    for (auto& sound : this->_activeSounds) {
        if (sound->getStatus() != sf::Sound::Playing) {
            return sound.get();
        }
    }

    // if all sounds used, stop the first one
    this->_activeSounds[0]->stop();
    return this->_activeSounds[0].get();
}

AudioAssets AudioManager::mapProtocolToAsset(protocol::AudioEffectType type) const
    {
        switch(type) {
            // WEAPONS
            case protocol::AudioEffectType::SFX_SHOOT_BASIC:
                return AudioAssets::SFX_SHOOT_BASIC;
            case protocol::AudioEffectType::SFX_SHOOT_CHARGED:
                return AudioAssets::SFX_SHOOT_CHARGED;
            case protocol::AudioEffectType::SFX_SHOOT_LASER:
                return AudioAssets::SFX_SHOOT_LASER;

            // EXPLOSIONS
            case protocol::AudioEffectType::SFX_EXPLOSION_SMALL_1:
                return AudioAssets::SFX_EXPLOSION_SMALL_1;
            case protocol::AudioEffectType::SFX_EXPLOSION_SMALL_2:
                return AudioAssets::SFX_EXPLOSION_SMALL_2;
            case protocol::AudioEffectType::SFX_EXPLOSION_LARGE_1:
                return AudioAssets::SFX_EXPLOSION_LARGE_1;
            case protocol::AudioEffectType::SFX_EXPLOSION_LARGE_2:
                return AudioAssets::SFX_EXPLOSION_LARGE_2;

            // POWERUPS
            case protocol::AudioEffectType::SFX_POWERUP_COLLECT_1:
                return AudioAssets::SFX_POWERUP_COLLECT_1;
            case protocol::AudioEffectType::SFX_POWERUP_COLLECT_2:
                return AudioAssets::SFX_POWERUP_COLLECT_2;

            // PLAYER
            case protocol::AudioEffectType::SFX_PLAYER_HIT:
                return AudioAssets::SFX_PLAYER_HIT;
            case protocol::AudioEffectType::SFX_PLAYER_DEATH_1:
                return AudioAssets::SFX_PLAYER_DEATH_1;
            case protocol::AudioEffectType::SFX_PLAYER_DEATH_2:
                return AudioAssets::SFX_PLAYER_DEATH_2;
            case protocol::AudioEffectType::SFX_PLAYER_DEATH_3:
                return AudioAssets::SFX_PLAYER_DEATH_3;

            // FORCE
            case protocol::AudioEffectType::SFX_FORCE_ATTACH:
                return AudioAssets::SFX_FORCE_ATTACH;
            case protocol::AudioEffectType::SFX_FORCE_DETACH:
                return AudioAssets::SFX_FORCE_DETACH;

            // BOSS
            case protocol::AudioEffectType::SFX_BOSS_ROAR:
                return AudioAssets::SFX_BOSS_ROAR;

            // UI
            case protocol::AudioEffectType::SFX_MENU_SELECT:
                return AudioAssets::SFX_MENU_SELECT;
            case protocol::AudioEffectType::SFX_MENU_ALERT:
                return AudioAssets::SFX_MENU_ALERT;

            // MUSIC
            case protocol::AudioEffectType::MAIN_MENU_MUSIC:
                return AudioAssets::MAIN_MENU_MUSIC;
            case protocol::AudioEffectType::FIRST_LEVEL_MUSIC:
                return AudioAssets::FIRST_LEVEL_MUSIC;
            case protocol::AudioEffectType::SECOND_LEVEL_MUSIC:
                return AudioAssets::SECOND_LEVEL_MUSIC;
            case protocol::AudioEffectType::THIRD_LEVEL_MUSIC:
                return AudioAssets::THIRD_LEVEL_MUSIC;
            case protocol::AudioEffectType::FOURTH_LEVEL_MUSIC:
                return AudioAssets::FOURTH_LEVEL_MUSIC;
            case protocol::AudioEffectType::VICTORY_MUSIC:
                return AudioAssets::VICTORY_MUSIC;
            case protocol::AudioEffectType::DEFEAT_MUSIC:
                return AudioAssets::DEFEAT_MUSIC;

            default:
                LOG_ERROR_CAT("[AudioManager] Unknown audio effect type: ", std::to_string(static_cast<int>(type)));
                return AudioAssets::SFX_SHOOT_BASIC; // Fallback
        }
    }

} // namespace audio