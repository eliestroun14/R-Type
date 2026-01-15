/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** AudioSystem
*/

#ifndef AUDIOSYSTEM_HPP_
#define AUDIOSYSTEM_HPP_

#include <engine/GameEngine.hpp>
#include <engine/ecs/system/System.hpp>

/**
 * @class AudioSystem
 * @brief Manages audio effects for entities with AudioSource or AudioEffect components.
 *
 * This system processes audio playback for:
 * - AudioSource: Continuous audio associated with entities (e.g., projectile impact sounds)
 * - AudioEffect: One-shot audio effects triggered at specific positions
 *
 * The system handles 3D positional audio attenuation and volume mixing.
 */
class AudioSystem : public System {
    public:
        AudioSystem(gameEngine::GameEngine& engine) : _engine(engine) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine& _engine;

        /**
         * @brief Convert AudioAssets enum to protocol::AudioEffectType
         */
        protocol::AudioEffectType getAudioEffectFromAsset(AudioAssets assetId);
};

#endif /* !AUDIOSYSTEM_HPP_ */
