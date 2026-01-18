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
 * @brief Manages audio playback for entities with AudioSource or AudioEffect components.
 *
 * **Purpose:**
 * This system processes all entities with AudioSource components and handles their
 * audio playback timing and 3D positional audio calculations. It bridges the gap between
 * game entities and the underlying audio engine.
 *
 * **Responsibilities:**
 * - Tracks playback state for each audio source (played/not played)
 * - Manages replay timing for looping sounds
 * - Converts AudioAssets to protocol audio effect types
 * - Handles 3D positional audio with distance attenuation
 * - Distinguishes between UI sounds and spatial sounds
 *
 * **How it works:**
 * 1. Each frame, onUpdate() iterates through all entities with AudioSource components
 * 2. For each entity, it checks if the sound should play based on:
 *    - `loop` flag: determines one-shot vs. repeating behavior
 *    - `hasBeenPlayed`: tracks if sound has already played
 *    - `elapsedTimeSincePlay`: determines replay timing
 *    - `soundDuration`: defines replay interval for looping sounds
 * 3. When a sound should play, it calls the GameEngine's playSound() or playSoundUI()
 * 4. Resets playback state for next cycle
 *
 * **Example Usage:**
 * @code
 * // In spawnProjectile():
 * Entity projectile = engine.createEntity("bullet");
 * engine.addComponent<Transform>(projectile, Transform(x, y, 0, 1));
 * engine.addComponent<Velocity>(projectile, Velocity(vx, vy));
 * 
 * // Add one-shot sound that plays once when projectile spawns
 * engine.addComponent<AudioSource>(projectile,
 *     AudioSource(AudioAssets::SFX_SHOOT_BASIC,  // asset
 *                 false,                         // loop=false (one-shot)
 *                 100.0f,                        // minDistance
 *                 0.5f,                          // attenuation
 *                 false,                         // isUI=false (3D audio)
 *                 0.3f));                        // soundDuration
 * @endcode
 *
 * @see AudioSource Component for audio properties
 * @see GameEngine::playSound() for 3D audio playback
 * @see GameEngine::playSoundUI() for UI audio playback
 */
class AudioSystem : public System {
    public:
        /**
         * @brief Constructor
         * @param engine Reference to the GameEngine for audio playback
         */
        AudioSystem(gameEngine::GameEngine& engine) : _engine(engine) {}

        /// @brief Called when system is created (not used for AudioSystem)
        void onCreate() override {}

        /**
         * @brief Update audio playback each frame
         * 
         * Processes all entities with AudioSource components and:
         * - Updates elapsed time since playback
         * - Determines if sound should play based on loop flag
         * - Plays sound via GameEngine if needed
         * - Resets playback state for timing
         *
         * @param dt Delta time since last frame (in seconds)
         *
         * @note This is called by the engine every frame automatically
         */
        void onUpdate(float dt) override;

    private:
        /// @brief Reference to GameEngine for audio operations
        gameEngine::GameEngine& _engine;

        /**
         * @brief Maps AudioAssets enum values to protocol::AudioEffectType
         *
         * Converts internal audio asset identifiers to network protocol identifiers
         * for consistency across audio systems.
         *
         * @param assetId The AudioAssets enum value to convert
         * @return Corresponding protocol::AudioEffectType value
         *
         * @note This ensures that all audio types are properly mapped between
         *       internal representation and protocol/network representation
         */
        protocol::AudioEffectType getAudioEffectFromAsset(AudioAssets assetId);
};

#endif /* !AUDIOSYSTEM_HPP_ */
