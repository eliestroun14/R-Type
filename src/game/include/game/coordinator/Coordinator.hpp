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
        void processCLientPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs);

        //TODO:
        void buildSeverPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs);
        void buildClientPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs);

        void handlePlayerInputPacket(const common::protocol::Packet& packet, uint64_t elapsedMs);

        void handlePacketCreateEntity(const common::protocol::Packet& packet);

        void handlePacketTransformSnapshot(const common::protocol::Packet& packet);

        void handleGameStart(const common::protocol::Packet& packet);
        void handleGameEnd(const common::protocol::Packet& packet);

    public:
        bool _gameRunning = false;

    private:
        PlayerSpriteAllocator _playerSpriteAllocator;

        std::shared_ptr<gameEngine::GameEngine> _engine;
};

#endif /* !COORDINATOR_HPP_ */
