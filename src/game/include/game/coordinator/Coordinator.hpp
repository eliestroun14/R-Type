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

        std::vector<uint32_t> getPlayablePlayerIds();
        bool createPacketInputClient(common::protocol::Packet *packet, uint32_t playerId);

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


        std::shared_ptr<gameEngine::GameEngine> getEngine() const;

    public:
        bool _gameRunning = false;

    private:
        PlayerSpriteAllocator _playerSpriteAllocator;

        std::shared_ptr<gameEngine::GameEngine> _engine;
};

#endif /* !COORDINATOR_HPP_ */
