/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** AudioSystem implementation
*/

#include "game/systems/AudioSystem.hpp"
#include <spdlog/spdlog.h>

void AudioSystem::onUpdate(float dt)
{
    // Get component arrays
    auto& audioSources = this->_engine.getComponents<AudioSource>();
    auto& transforms = this->_engine.getComponents<Transform>();

    // Process entities with AudioSource components
    // These are sounds attached to entities (e.g., projectile sounds)
    for (size_t e : this->_entities) {
        if (!audioSources[e] || !transforms[e]) {
            continue;
        }

        try {
            auto& audioSource = audioSources[e].value();
            auto& transform = transforms[e].value();

            // Update elapsed time
            audioSource.elapsedTimeSincePlay += dt;

            // Determine if we should play the sound
            bool shouldPlay = false;
            
            if (audioSource.playOnce) {
                // Play only once
                shouldPlay = !audioSource.hasBeenPlayed;
            } else {
                // Play repeatedly when the sound finishes
                shouldPlay = !audioSource.hasBeenPlayed || 
                           audioSource.elapsedTimeSincePlay >= audioSource.soundDuration;
            }

            if (shouldPlay) {
                // Convert AudioAssets to protocol::AudioEffectType
                protocol::AudioEffectType effectType = getAudioEffectFromAsset(audioSource.assetId);

                if (audioSource.isUI) {
                    // UI sound (no 3D positioning)
                    this->_engine.playSoundUI(effectType, 1.0f, 1.0f);
                    spdlog::debug("AudioSystem: Playing UI sound for asset {}", static_cast<int>(audioSource.assetId));
                } else {
                    // 3D audio with positional attenuation
                    this->_engine.playSound(effectType, transform.x, transform.y, 1.0f, 1.0f);
                    spdlog::debug("AudioSystem: Playing 3D sound '{}' at position ({}, {})",
                                static_cast<int>(audioSource.assetId), transform.x, transform.y);
                }

                // Mark as played and reset timer for next cycle
                audioSource.hasBeenPlayed = true;
                audioSource.elapsedTimeSincePlay = 0.0f;
            }
        } catch (const std::exception& e) {
            spdlog::error("AudioSystem: Error processing AudioSource: {}", e.what());
        }
    }
}

protocol::AudioEffectType AudioSystem::getAudioEffectFromAsset(AudioAssets assetId)
{
    // Map AudioAssets enum to protocol::AudioEffectType
    switch (assetId) {
        case AudioAssets::SFX_SHOOT_BASIC:
            return protocol::AudioEffectType::SFX_SHOOT_BASIC;
        case AudioAssets::SFX_SHOOT_CHARGED:
            return protocol::AudioEffectType::SFX_SHOOT_CHARGED;
        case AudioAssets::SFX_SHOOT_LASER:
            return protocol::AudioEffectType::SFX_SHOOT_LASER;
        case AudioAssets::SFX_EXPLOSION_SMALL_1:
            return protocol::AudioEffectType::SFX_EXPLOSION_SMALL_1;
        case AudioAssets::SFX_EXPLOSION_SMALL_2:
            return protocol::AudioEffectType::SFX_EXPLOSION_SMALL_2;
        case AudioAssets::SFX_EXPLOSION_LARGE_1:
            return protocol::AudioEffectType::SFX_EXPLOSION_LARGE_1;
        case AudioAssets::SFX_EXPLOSION_LARGE_2:
            return protocol::AudioEffectType::SFX_EXPLOSION_LARGE_2;
        case AudioAssets::SFX_POWERUP_COLLECT_1:
            return protocol::AudioEffectType::SFX_POWERUP_COLLECT_1;
        case AudioAssets::SFX_POWERUP_COLLECT_2:
            return protocol::AudioEffectType::SFX_POWERUP_COLLECT_2;
        case AudioAssets::SFX_PLAYER_HIT:
            return protocol::AudioEffectType::SFX_PLAYER_HIT;
        case AudioAssets::SFX_PLAYER_DEATH_1:
            return protocol::AudioEffectType::SFX_PLAYER_DEATH_1;
        case AudioAssets::SFX_PLAYER_DEATH_2:
            return protocol::AudioEffectType::SFX_PLAYER_DEATH_2;
        case AudioAssets::SFX_PLAYER_DEATH_3:
            return protocol::AudioEffectType::SFX_PLAYER_DEATH_3;
        case AudioAssets::SFX_FORCE_ATTACH:
            return protocol::AudioEffectType::SFX_FORCE_ATTACH;
        case AudioAssets::SFX_FORCE_DETACH:
            return protocol::AudioEffectType::SFX_FORCE_DETACH;
        case AudioAssets::SFX_BOSS_ROAR:
            return protocol::AudioEffectType::SFX_BOSS_ROAR;
        case AudioAssets::SFX_MENU_SELECT:
            return protocol::AudioEffectType::SFX_MENU_SELECT;
        case AudioAssets::SFX_MENU_ALERT:
            return protocol::AudioEffectType::SFX_MENU_ALERT;
        default:
            return protocol::AudioEffectType::SFX_SHOOT_BASIC;
    }
}
