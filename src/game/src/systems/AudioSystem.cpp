/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** AudioSystem implementation
*/

#include "game/systems/AudioSystem.hpp"
#include <spdlog/spdlog.h>

/**
 * @brief Update audio playback for all entities with AudioSource components
 *
 * **Algorithm:**
 * 1. Get references to AudioSource and Transform component arrays
 * 2. For each entity with both components:
 *    a. Increment elapsed time since last play
 *    b. Determine if sound should play based on:
 *       - loop flag (false = one-shot, true = repeating)
 *       - hasBeenPlayed state
 *       - elapsed time vs. sound duration (for replay)
 *    c. If sound should play:
 *       - Determine audio type from AssetId
 *       - Play via 3D audio (with position) or UI audio (no position)
 *       - Mark as played and reset elapsed time
 *
 * **One-Shot Behavior (loop=false):**
 * - Sound plays once when component is added
 * - Never plays again, even if entity persists
 * - Typical use: bullet hit sounds, explosions
 *
 * **Looping Behavior (loop=true):**
 * - Sound plays initially
 * - After soundDuration seconds, it replays
 * - Repeats indefinitely until component removed
 * - Typical use: ambient loops, engine rumble
 *
 * @param dt Delta time since last frame update (in milliseconds)
 *
 * @note Only processes entities in this->_entities (set by engine)
 * @note Safely skips entities missing components
 * @see AudioSource for component details
 */
void AudioSystem::onUpdate(float dt)
{
    // Get component arrays
    auto& audioSources = this->_engine.getComponents<AudioSource>();
    auto& transforms = this->_engine.getComponents<Transform>();
    auto& configs = this->_engine.getComponents<GameConfig>();

    bool musicEnabled = true;
    bool soundEnabled = true;

    for (auto& config : configs)
        if (config.has_value()) {
            if (config->musicEnabled == false && config->soundEnabled == false)
                return;
            musicEnabled = config->musicEnabled;
            soundEnabled = config->soundEnabled;
        }

    // Process entities with AudioSource components
    // These are sounds attached to entities (e.g., projectile sounds)
    for (size_t e = 0; e < audioSources.size(); e++) {
        if (!audioSources[e])
            continue;

        try {
            auto& audioSource = audioSources[e].value();

            // check game config settings
            if (!audioSource.loop && !soundEnabled)
                continue;
            if (audioSource.loop && !musicEnabled)
                continue;

            // handle transform (not for UI sounds)
            float posX = 0, posY = 0;
            if (!audioSource.isUI) {
                if (e < transforms.size() && transforms[e].has_value()) {
                    posX = transforms[e]->x;
                    posY = transforms[e]->y;
                } else
                    continue; // if there is no transform for a 3d sound, we do not play it
            }

            audioSource.elapsedTimeSincePlay += dt;

            // Determine if we should play the sound
            bool shouldPlay = false;

            if (!audioSource.loop) {
                // Play only once (loop=false means one-shot)
                shouldPlay = !audioSource.hasBeenPlayed;
            } else {
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
                    this->_engine.playSound(effectType, posX, posY, 1.0f, 1.0f);
                    spdlog::debug("AudioSystem: Playing 3D sound '{}' at position ({}, {})",
                                static_cast<int>(audioSource.assetId), posX, posY);
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
    /**
     * @brief Maps internal AudioAssets enum to protocol audio effect types
     *
     * This function provides a conversion layer between the engine's internal
     * audio asset identifiers and the network protocol's audio effect types,
     * ensuring consistency across all audio systems.
     *
     * **Mapping Strategy:**
     * - Each AudioAssets entry has a corresponding protocol::AudioEffectType
     * - Maintains 1:1 correspondence for proper audio playback
     * - Unknown assets return SFX_SHOOT_BASIC as fallback
     *
     * @param assetId The internal AudioAssets enum value to convert
     * @return The corresponding protocol::AudioEffectType for network/playback use
     */
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
