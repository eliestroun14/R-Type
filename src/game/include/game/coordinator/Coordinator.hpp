/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Coordinator
*/

#ifndef COORDINATOR_HPP_
#define COORDINATOR_HPP_

#include <memory>
#include <string>

#include <engine/GameEngine.hpp>
#include <engine/ecs/component/Components.hpp>
#include <common/protocol/PacketManager.hpp>
#include <common/protocol/Protocol.hpp>
#include <cstring>
#include <common/constants/defines.hpp>
#include <common/constants/render/Assets.hpp>
#include <common/logger/Logger.hpp>
#include <common/protocol/Payload.hpp>
#include <engine/render/SpriteAllocator.hpp>


class Coordinator {
    public:
        struct ProjectileInfo {
            uint32_t id;
            float x;
            float y;
            float velX;
            float velY;
        };

        Coordinator() {
            std::cout << "Coordinator constructor START" << std::endl;
            std::cout << "Coordinator constructor END" << std::endl;
        }
        ~Coordinator() = default;

        // ==============================================================
        //                          Initialization
        // ==============================================================

        void initEngine();
        void initEngineRender();

        //TODO:
        void processServerPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs);
        void processClientPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs);

        //TODO:
        void buildSeverPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs);
        void buildClientPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs);

        // HANDLE PACKETS

        void handlePlayerInputPacket(const common::protocol::Packet& packet, uint64_t elapsedMs);

        void handlePacketCreateEntity(const common::protocol::Packet& packet);
        void handlePacketDestroyEntity(const common::protocol::Packet& packet);
        void handlePacketTransformSnapshot(const common::protocol::Packet& packet);
        void handlePacketHealthSnapshot(const common::protocol::Packet& packet);
        void handlePacketWeaponSnapshot(const common::protocol::Packet& packet);
        void handlePacketAnimationSnapshot(const common::protocol::Packet& packet);
        void handlePacketComponentRemove(const common::protocol::Packet& packet);
        void handlePacketTransformSnapshotDelta(const common::protocol::Packet& packet);
        void handlePacketHealthSnapshotDelta(const common::protocol::Packet& packet);
        void handlePacketPlayerHit(const common::protocol::Packet& packet);
        void handlePacketPlayerDeath(const common::protocol::Packet& packet);
        void handlePacketScoreUpdate(const common::protocol::Packet& packet);
        void handlePacketPowerupPickup(const common::protocol::Packet& packet);
        void handlePacketWeaponFire(const common::protocol::Packet& packet);
        void handlePacketVisualEffect(const common::protocol::Packet& packet);
        void handlePacketAudioEffect(const common::protocol::Packet& packet);
        void handlePacketParticleSpawn(const common::protocol::Packet& packet);
        void handleGameStart(const common::protocol::Packet& packet);
        void handleGameEnd(const common::protocol::Packet& packet);
        void handlePacketLevelComplete(const common::protocol::Packet& packet);
        void handlePacketLevelStart(const common::protocol::Packet& packet);
        void handlePacketForceState(const common::protocol::Packet& packet);

        /**
         * @brief Handles an AI_STATE packet from the server
         *
         * This method processes AI state updates for enemy entities. It parses the packet
         * data to extract the AI entity's current state, behavior type, target information,
         * and waypoint coordinates, then updates the corresponding AI component in the
         * game engine.
         *
         * @param packet The AI_STATE packet containing:
         *        - entity_id (uint32_t): The ID of the AI entity to update
         *        - current_state (uint8_t): The current AI state (idle, patrol, chase, etc.)
         *        - behavior_type (uint8_t): The behavior pattern (AIBehaviorType enum)
         *        - target_entity_id (uint32_t): The ID of the current target entity (0 if no target)
         *        - waypoint_x (int16_t): X coordinate of the next waypoint
         *        - waypoint_y (int16_t): Y coordinate of the next waypoint
         *        - state_timer (uint16_t): Time remaining in the current state (in milliseconds)
         *
         * @note Packet format is exactly AI_STATE_PAYLOAD_SIZE (18 bytes)
         * @note If the entity doesn't exist in the engine, the packet is ignored
         * @note If the AI component doesn't exist for the entity, the packet is ignored
         *
         * @see AI_STATE_PAYLOAD_SIZE
         * @see AIBehaviorType
         */
        void handlePacketAIState(const common::protocol::Packet& packet);

        // ==============================================================
        //                          CREATE PACKET 
        // ==============================================================

        /**
         * @brief Create an ENTITY_SPAWN packet
         * @param packet Pointer to the packet to fill
         * @param entityId The entity ID
         * @param entityType The entity type (from EntityTypes enum)
         * @param posX X position
         * @param posY Y position
         * @param mobVariant Mob variant/subtype
         * @param initialHealth Initial health
         * @param initialVelX Initial velocity X
         * @param initialVelY Initial velocity Y
         * @param isPlayable Whether the entity is playable (0 or 1)
         */
        void createEntitySpawnPacket(
            common::protocol::Packet* packet,
            uint32_t entityId,
            uint8_t entityType,
            uint16_t posX,
            uint16_t posY,
            uint8_t mobVariant,
            uint8_t initialHealth,
            int16_t initialVelX,
            int16_t initialVelY,
            uint8_t isPlayable
        );

        /**
         * @brief Create an ENTITY_DESTROY packet
         * @param packet Pointer to the packet to fill
         * @param entityId The entity ID to destroy
         * @param destroyReason Reason for destruction (from EntityDestroyReasons enum)
         * @param finalPosX Final X position
         * @param finalPosY Final Y position
         */
        void createEntityDestroyPacket(
            common::protocol::Packet* packet,
            uint32_t entityId,
            uint8_t destroyReason,
            uint16_t finalPosX,
            uint16_t finalPosY
        );

        /**
         * @brief Create a TRANSFORM_SNAPSHOT packet
         * @param packet Pointer to the packet to fill
         * @param worldTick Current server tick
         * @param transforms Vector of entity IDs and their transform components
         */
        void createTransformSnapshotPacket(
            common::protocol::Packet* packet,
            uint32_t worldTick,
            const std::vector<std::pair<uint32_t, protocol::ComponentTransform>>& transforms
        );

        /**
         * @brief Create a HEALTH_SNAPSHOT packet
         * @param packet Pointer to the packet to fill
         * @param worldTick Current server tick
         * @param healthData Vector of entity IDs and their health components
         */
        void createHealthSnapshotPacket(
            common::protocol::Packet* packet,
            uint32_t worldTick,
            const std::vector<std::pair<uint32_t, protocol::ComponentHealth>>& healthData
        );

        /**
         * @brief Create a WEAPON_SNAPSHOT packet
         * @param packet Pointer to the packet to fill
         * @param worldTick Current server tick
         * @param weaponData Vector of entity IDs and their weapon components
         */
        void createWeaponSnapshotPacket(
            common::protocol::Packet* packet,
            uint32_t worldTick,
            const std::vector<std::pair<uint32_t, protocol::ComponentWeapon>>& weaponData
        );

        /**
         * @brief Create an ANIMATION_SNAPSHOT packet
         * @param packet Pointer to the packet to fill
         * @param worldTick Current server tick
         * @param animationData Vector of entity IDs and their animation components
         */
        void createAnimationSnapshotPacket(
            common::protocol::Packet* packet,
            uint32_t worldTick,
            const std::vector<std::pair<uint32_t, protocol::ComponentAnimation>>& animationData
        );

        /**
         * @brief Create a PLAYER_HIT packet
         * @param packet Pointer to the packet to fill
         * @param playerId Player who was hit
         * @param attackerId Entity that caused damage
         * @param damage Damage amount
         * @param remainingHealth Health after damage
         * @param remainingShield Shield after damage
         * @param hitPosX Hit location X
         * @param hitPosY Hit location Y
         */
        void createPlayerHitPacket(
            common::protocol::Packet* packet,
            uint32_t playerId,
            uint32_t attackerId,
            uint8_t damage,
            uint8_t remainingHealth,
            uint8_t remainingShield,
            int16_t hitPosX,
            int16_t hitPosY
        );

        /**
         * @brief Create a PLAYER_DEATH packet
         * @param packet Pointer to the packet to fill
         * @param playerId Player who died
         * @param killerId Entity that killed the player
         * @param scoreBeforeDeath Player's score
         * @param deathPosX Death location X
         * @param deathPosY Death location Y
         */
        void createPlayerDeathPacket(
            common::protocol::Packet* packet,
            uint32_t playerId,
            uint32_t killerId,
            uint32_t scoreBeforeDeath,
            int16_t deathPosX,
            int16_t deathPosY
        );

        /**
         * @brief Create a SCORE_UPDATE packet
         * @param packet Pointer to the packet to fill
         * @param playerId Player whose score changed
         * @param newScore New total score
         * @param scoreDelta Change in score (can be negative)
         * @param reason Reason for score change (from ScoreChangeReasons enum)
         */
        void createScoreUpdatePacket(
            common::protocol::Packet* packet,
            uint32_t playerId,
            uint32_t newScore,
            int16_t scoreDelta,
            uint8_t reason
        );

        /**
         * @brief Create a POWER_PICKUP packet
         * @param packet Pointer to the packet to fill
         * @param playerId Player who picked up the powerup
         * @param powerupId Powerup entity ID
         * @param powerupType Type of powerup (from PowerupTypes enum)
         * @param duration Effect duration in seconds (0 = permanent)
         */
        void createPowerupPickupPacket(
            common::protocol::Packet* packet,
            uint32_t playerId,
            uint32_t powerupId,
            uint8_t powerupType,
            uint8_t duration
        );

        /**
         * @brief Create a WEAPON_FIRE packet
         * @param packet Pointer to the packet to fill
         * @param shooterId Entity that fired
         * @param projectileId New projectile entity ID
         * @param originX Fire origin X
         * @param originY Fire origin Y
         * @param directionX Direction vector X (normalized*1000)
         * @param directionY Direction vector Y (normalized*1000)
         * @param weaponType Weapon type fired (from WeaponTypes enum)
         */
        void createWeaponFirePacket(
            common::protocol::Packet* packet,
            uint32_t shooterId,
            uint32_t projectileId,
            int16_t originX,
            int16_t originY,
            int16_t directionX,
            int16_t directionY,
            uint8_t weaponType
        );

        /**
         * @brief Create a VISUAL_EFFECT packet
         * @param packet Pointer to the packet to fill
         * @param effectType Effect type (from VisualEffectType enum)
         * @param posX Position X
         * @param posY Position Y
         * @param durationMs Effect duration in milliseconds
         * @param scale Scale multiplier (100 = 1.0x)
         * @param colorR Red color component
         * @param colorG Green color component
         * @param colorB Blue color component
         */
        void createVisualEffectPacket(
            common::protocol::Packet* packet,
            uint8_t effectType,
            int16_t posX,
            int16_t posY,
            uint16_t durationMs,
            uint8_t scale,
            uint8_t colorR,
            uint8_t colorG,
            uint8_t colorB
        );

        /**
         * @brief Create an AUDIO_EFFECT packet
         * @param packet Pointer to the packet to fill
         * @param effectType Effect type (from AudioEffectType enum)
         * @param posX Position X (for 3D audio)
         * @param posY Position Y (for 3D audio)
         * @param volume Volume (0-255)
         * @param pitch Pitch modifier (100 = normal)
         */
        void createAudioEffectPacket(
            common::protocol::Packet* packet,
            uint8_t effectType,
            int16_t posX,
            int16_t posY,
            uint8_t volume,
            uint8_t pitch
        );

        /**
         * @brief Create a PARTICLE_SPAWN packet
         * @param packet Pointer to the packet to fill
         * @param particleSystemId Particle system type
         * @param posX Position X
         * @param posY Position Y
         * @param velocityX Initial velocity X
         * @param velocityY Initial velocity Y
         * @param particleCount Number of particles
         * @param lifetimeMs Particle lifetime in milliseconds
         * @param colorStartR Start color red
         * @param colorStartG Start color green
         * @param colorStartB Start color blue
         * @param colorEndR End color red (for fade)
         * @param colorEndG End color green (for fade)
         * @param colorEndB End color blue (for fade)
         */
        void createParticleSpawnPacket(
            common::protocol::Packet* packet,
            uint16_t particleSystemId,
            int16_t posX,
            int16_t posY,
            int16_t velocityX,
            int16_t velocityY,
            uint16_t particleCount,
            uint16_t lifetimeMs,
            uint8_t colorStartR,
            uint8_t colorStartG,
            uint8_t colorStartB,
            uint8_t colorEndR,
            uint8_t colorEndG,
            uint8_t colorEndB
        );

        /**
         * @brief Create a GAME_START packet
         * @param packet Pointer to the packet to fill
         * @param gameInstanceId Game instance identifier
         * @param playerCount Number of players
         * @param playerIds Array of player IDs (up to 4)
         * @param levelId Starting level
         * @param difficulty Difficulty setting (from DifficultyLevels enum)
         */
        void createGameStartPacket(
            common::protocol::Packet* packet,
            uint32_t gameInstanceId,
            uint8_t playerCount,
            const uint32_t playerIds[4],
            uint8_t levelId,
            uint8_t difficulty
        );

        /**
         * @brief Create a GAME_END packet
         * @param packet Pointer to the packet to fill
         * @param endReason Reason game ended (from GameEndReasons enum)
         * @param finalScores Final scores for all players (array of 4)
         * @param winnerId Winning player ID
         * @param playTime Total game time in seconds
         */
        void createGameEndPacket(
            common::protocol::Packet* packet,
            uint8_t endReason,
            const uint32_t finalScores[4],
            uint8_t winnerId,
            uint32_t playTime
        );

        /**
         * @brief Create a LEVEL_COMPLETE packet
         * @param packet Pointer to the packet to fill
         * @param completedLevel Level that was completed
         * @param nextLevel Next level to load (0xFF = game end)
         * @param bonusScore Completion bonus
         * @param completionTime Time taken in seconds
         */
        void createLevelCompletePacket(
            common::protocol::Packet* packet,
            uint8_t completedLevel,
            uint8_t nextLevel,
            uint32_t bonusScore,
            uint16_t completionTime
        );

        /**
         * @brief Create a LEVEL_START packet
         * @param packet Pointer to the packet to fill
         * @param levelId Level identifier
         * @param levelName Level name (max 32 chars)
         * @param estimatedDuration Estimated time in seconds
         */
        void createLevelStartPacket(
            common::protocol::Packet* packet,
            uint8_t levelId,
            const char* levelName,
            uint16_t estimatedDuration
        );

        /**
         * @brief Create a FORCE_STATE packet (R-Type signature weapon)
         * @param packet Pointer to the packet to fill
         * @param forceEntityId The Force entity ID
         * @param parentShipId Ship it's attached to (0 = detached)
         * @param attachmentPoint Where it's attached (from ForceAttachmentPoint enum)
         * @param powerLevel Power level (1-5)
         * @param chargePercentage Charge percentage (0-100)
         * @param isFiring Boolean flag (0 or 1)
         */
        void createForceStatePacket(
            common::protocol::Packet* packet,
            uint32_t forceEntityId,
            uint32_t parentShipId,
            uint8_t attachmentPoint,
            uint8_t powerLevel,
            uint8_t chargePercentage,
            uint8_t isFiring
        );

        /**
         * @brief Create an AI_STATE packet
         * @param packet Pointer to the packet to fill
         * @param entityId AI entity ID
         * @param currentState Current AI state
         * @param behaviorType Behavior pattern (from AIBehaviorType enum)
         * @param targetEntityId Current target (0 = no target)
         * @param waypointX Next waypoint X
         * @param waypointY Next waypoint Y
         * @param stateTimer Time remaining in current state (milliseconds)
         */
        void createAIStatePacket(
            common::protocol::Packet* packet,
            uint32_t entityId,
            uint8_t currentState,
            uint8_t behaviorType,
            uint32_t targetEntityId,
            int16_t waypointX,
            int16_t waypointY,
            uint16_t stateTimer
        );

        std::shared_ptr<gameEngine::GameEngine> getEngine() const;

    public:
        bool _gameRunning = false;

    private:
        PlayerSpriteAllocator _playerSpriteAllocator;

        std::shared_ptr<gameEngine::GameEngine> _engine;
};

#endif /* !COORDINATOR_HPP_ */
