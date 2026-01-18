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
#include <functional>
#include <deque>

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
#include <game/systems/RenderSystem.hpp>
#include <game/systems/PlayerSystem.hpp>
#include <game/systems/MovementSystem.hpp>
#include <game/systems/ShootSystem.hpp>
#include <set>
#include <game/systems/ButtonSystem.hpp>
#include <game/systems/AnimationSystem.hpp>
#include <game/systems/AccessibilitySystem.hpp>
#include <game/systems/BackgroundSystem.hpp>
#include <game/systems/RebindSystem.hpp>


class Coordinator {
    public:
        struct ProjectileInfo {
            uint32_t id;
            float x;
            float y;
            float velX;
            float velY;
        };

        Coordinator() : _isServer(false) {
            std::cout << "Coordinator constructor START" << std::endl;
            std::cout << "Coordinator constructor END" << std::endl;
        }
        ~Coordinator() = default;

        // ==============================================================
        //                          Initialization
        // ==============================================================

        void initEngine();
        void initEngineRender();
        
        /** @brief Sets whether this coordinator is running on the server. */
        void setIsServer(bool isServer) { _isServer = isServer; }
        
        /** @brief Returns whether this coordinator is running on the server. */
        bool isServer() const { return _isServer; }
        
        /** @brief Set a callback to notify when players are ready/not ready. */
        void setGameNotificationCallback(std::function<void(uint32_t, bool)> callback) {
            _gameNotificationCallback = callback;
        }
        
        /** @brief Set a callback to notify when level starts (client-side). */
        void setLevelStartCallback(std::function<void()> callback) {
            _levelStartCallback = callback;
        }

        // ==============================================================
        //                       Entity creation helpers
        // ==============================================================

        Entity createPlayerEntity(
            uint32_t playerId,
            float posX,
            float posY,
            float velX,
            float velY,
            uint16_t initialHealth,
            bool isPlayable,
            bool withRenderComponents = true
        );

        Entity createScoreEntity(
            uint32_t scoreId,
            float posX,
            float posY,
            uint32_t initialScore
        );

        Entity createEnemyEntity(
            uint32_t enemyId,
            float posX,
            float posY,
            float velX,
            float velY,
            uint16_t initialHealth,
            bool withRenderComponents = true
        );

        /**
         * @brief Creates a projectile entity
         * 
         * IMPORTANT: Projectiles do NOT receive NetworkId components and are NOT
         * synchronized via transform snapshots. They have predictable linear movement
         * and are spawned on clients via WEAPON_FIRE packets containing initial
         * position and velocity. Clients simulate movement locally.
         * 
         * @param projectileId Unique identifier for the projectile
         * @param posX Initial X position
         * @param posY Initial Y position
         * @param velX X velocity (pixels per second)
         * @param velY Y velocity (pixels per second)
         * @param isPlayerProjectile True if fired by a player, false if by an enemy
         * @param damage Damage dealt on hit
         * @param withRenderComponents Whether to add visual components (client-side)
         * @return The created projectile entity
         */
        Entity createProjectileEntity(
            uint32_t projectileId,
            float posX,
            float posY,
            float velX,
            float velY,
            bool isPlayerProjectile,
            uint16_t damage = 10,
            bool withRenderComponents = true
        );

        //TODO:
        void processServerPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs);
        void processClientPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs);

        /**
         * @brief Generates snapshot packets for all networked entities
         * 
         * Creates transform, health, and weapon snapshots for entities that have
         * NetworkId components. Projectiles are intentionally excluded as they don't
         * have NetworkId - they're spawned via WEAPON_FIRE packets and simulated
         * locally on clients to reduce network bandwidth.
         * 
         * Also processes pending weapon fire events and generates WEAPON_FIRE packets.
         * 
         * @param outgoingPackets Vector to append generated packets to
         * @param elapsedMs Milliseconds since last tick
         */
        void buildServerPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs);
        void buildClientPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs);

        // Helper to spawn a player entity and broadcast ENTITY_SPAWN packet
        std::optional<common::protocol::Packet> spawnPlayerOnServer(uint32_t playerId, float posX, float posY);

        /** @brief Mark an entity as already broadcasted to prevent duplicate ENTITY_SPAWN packets.
         * Used when manually sending ENTITY_SPAWN packets instead of relying on the broadcast system.
         */
        void markEntityAsBroadcasted(uint32_t entityId);

        /** @brief Check if a PLAYER_INPUT packet should be sent to a specific player.
         * Returns false if the packet is a PLAYER_INPUT from that player (they shouldn't receive their own relayed input).
         * This prevents double-processing of input on the sender's client.
         */
        bool shouldSendPacketToPlayer(const common::protocol::Packet& packet, uint32_t targetPlayerId) const;

        // HANDLE PACKETS
        std::vector<uint32_t> getPlayablePlayerIds();
        
        /** @brief Get all connected player IDs (used by server to relay packets).
         * Returns all entities with InputComponent, which represents all players (including remote ones).
         */
        std::vector<uint32_t> getAllConnectedPlayerIds() const;
        
        bool createPacketInputClient(common::protocol::Packet *packet, uint32_t playerId);

        /** @brief Queue a weapon fire event to be processed into packets.
         * Called by ShootSystem when an entity fires.
         */
        void queueWeaponFire(uint32_t shooterId, float originX, float originY,
                            float directionX, float directionY, uint8_t weaponType);

        // HANDLE PACKETS
        void handlePlayerInputPacket(const common::protocol::Packet& packet, uint64_t elapsedMs);
        
        /** @brief Handle relayed PLAYER_INPUT packets from other clients.
         * This directly updates InputComponent without calling setPlayerInputAction
         * to avoid side effects and race conditions with animation logic.
         */
        void handleRelayedPlayerInputPacket(const common::protocol::Packet& packet, uint64_t elapsedMs);

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

        void handlePacketPlayerIsReady(const common::protocol::Packet& packet);
        void handlePacketPlayerNotReady(const common::protocol::Packet& packet);

        /** @brief Check if all players are ready to start the level.
         * @param connectedPlayers List of connected player IDs
         * @param maxPlayers Maximum number of players required
         * @param playerReadyStatus Map of player ready status
         * @return true if all connected players are ready and max players reached, false otherwise
         */
        bool areAllPlayersReady(
            const std::vector<uint32_t>& connectedPlayers,
            uint32_t maxPlayers,
            const std::unordered_map<uint32_t, bool>& playerReadyStatus
        ) const;
        
        /** @brief Queue a player ready event (client-side). */
        void queuePlayerIsReady(uint32_t playerId);
        
        /** @brief Queue a player not ready event (client-side). */
        void queuePlayerNotReady(uint32_t playerId);

        /** @brief Get the GameEngine instance for direct access. */
        std::shared_ptr<gameEngine::GameEngine> getEngine() { return _engine; }

        /** @brief Create and initialize a level entity (server-side only).
         * @param levelNumber The level number (used for naming).
         * @param duration Level duration in seconds (0 = infinite/until all waves complete).
         * @param backgroundAsset Asset path for the scrolling background.
         * @param soundTheme Asset path for the background music.
         * @return The created level entity.
         */
        Entity createLevelEntity(int levelNumber, float duration, const std::string& backgroundAsset, const std::string& soundTheme);

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

        // CREATE PACKET

        bool createPacketEntitySpawn(common::protocol::Packet* packet, uint32_t entityId, uint32_t sequence_number);
        bool createPacketTransformSnapshot(common::protocol::Packet* packet, const std::vector<uint32_t>& entityIds, uint32_t sequence_number);
        bool createPacketHealthSnapshot(common::protocol::Packet* packet, const std::vector<uint32_t>& entityIds, uint32_t sequence_number);
        bool createPacketWeaponSnapshot(common::protocol::Packet* packet, const std::vector<uint32_t>& entityIds, uint32_t sequence_number);

        /**
         * @brief Creates an ENTITY_DESTROY packet
         * @param packet Pointer to the packet to be filled
         * @param entityId The entity ID to destroy
         * @param reason Destruction reason code
         * @param sequence_number The global tick count since program started
         * @return true if packet was created successfully, false otherwise
         */
        bool createPacketEntityDestroy(common::protocol::Packet* packet, uint32_t entityId, uint8_t reason, uint32_t sequence_number);

        std::shared_ptr<gameEngine::GameEngine> getEngine() const;

    public:
        bool _gameRunning = false;
        
        // Callback to notify Game of player ready/not ready status
        std::function<void(uint32_t, bool)> _gameNotificationCallback;
        
        // Callback to notify Game when level starts (client-side)
        std::function<void()> _levelStartCallback;
        
    private:
        // Event structure for player ready/not ready
        struct PlayerReadyEvent {
            uint32_t playerId;
            bool isReady;
        };
        
        // Queue of pending player ready/not ready events (client-side)
        std::deque<PlayerReadyEvent> _pendingPlayerReadyEvents;

    private:
        void setupPlayerEntity(
            Entity entity,
            uint32_t playerId,
            float posX,
            float posY,
            float velX,
            float velY,
            uint16_t initialHealth,
            bool isPlayable,
            bool withRenderComponents
        );

        void setupScoreEntity(
            Entity entity,
            uint32_t /*scoreId*/,
            float posX,
            float posY,
            uint32_t initialScore
        );

        void setupEnemyEntity(
            Entity entity,
            uint32_t enemyId,
            float posX,
            float posY,
            float velX,
            float velY,
            uint16_t initialHealth,
            bool withRenderComponents
        );

        void setupProjectileEntity(
            Entity entity,
            uint32_t projectileId,
            float posX,
            float posY,
            float velX,
            float velY,
            bool isPlayerProjectile,
            uint16_t damage,
            bool withRenderComponents
        );


        /** @brief Displays everything that was drawn during the frame.
             * @param type The player entity.
             * @param x,y The action to check.
             * @param scale The action to check.
             * @param duration The action to check.
             * @param r,g,b The action to check.
            */
        void spawnVisualEffect(protocol::VisualEffectType type, float x, float y,
                                float scale, float duration,
                                float color_r, float color_g, float color_b);

        /** @brief Create an projectile entity.
         * @param shooter The shooter entity.
         * @param projectile_id The projectile entity.
         * @param weapon_type The type of weapon to know the type of projectile.
         * @param origin_x,origin_y Origins of the projectile.
         * @param dir_x,dir_y Directions of the projectile.
        */
        Entity spawnProjectile(Entity shooter, uint32_t projectile_id, uint8_t weapon_type,
                                float origin_x, float origin_y,
                                float dir_x, float dir_y);


        /** @brief Plays an audio effect at a specific position with volume and pitch.
         * @param type The type of audio effect to play.
         * @param x,y The position where the sound originates.
         * @param volume The volume level (0.0 - 1.0).
         * @param pitch The pitch modifier (1.0 = normal).
         */
        void playAudioEffect(protocol::AudioEffectType type, float x, float y,
                            float volume, float pitch);

        /** @brief Plays a music. */
        void playMusic(protocol::AudioEffectType musicType);

        /** @brief Stops a music. */
        void stopMusic();

        // ==============================================================
        //                      Weapon Fire Events
        // ==============================================================

        struct WeaponFireEvent {
            uint32_t shooterId;
            uint32_t projectileId;
            float originX;
            float originY;
            float directionX;
            float directionY;
            uint8_t weaponType;
        };

        struct RelayPacket {
            common::protocol::Packet packet;
            uint32_t sourcePlayerId;  // The player who sent this packet - should NOT receive it
        };

    private:
        PlayerSpriteAllocator _playerSpriteAllocator;

        std::shared_ptr<gameEngine::GameEngine> _engine;
        
        // Server/Client flag
        bool _isServer;
        
        // Queue of weapon fire events to process
        std::vector<WeaponFireEvent> _pendingWeaponFires;
        
        // Queue of PLAYER_INPUT packets to relay to other clients (but NOT to the source player)
        std::vector<RelayPacket> _packetsToRelay;
        
        // Track entities that have been broadcast to clients (server-side only)
        // Used to send initial ENTITY_SPAWN packets for newly created networked entities
        std::set<uint32_t> _broadcastedEntityIds;
};

#endif /* !COORDINATOR_HPP_ */
