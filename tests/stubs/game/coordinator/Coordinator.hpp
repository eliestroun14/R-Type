/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Stub Coordinator for Game.cpp coverage tests
*/

#ifndef TESTS_STUB_COORDINATOR_HPP_
#define TESTS_STUB_COORDINATOR_HPP_

#include <memory>
#include <vector>

#include <engine/GameEngine.hpp>
#include <common/constants/defines.hpp>
#include <common/protocol/Packet.hpp>
#include <common/protocol/Protocol.hpp>

class Coordinator {
public:
    Coordinator() : _engine(std::make_shared<gameEngine::GameEngine>()) {}
    ~Coordinator() = default;

    void initEngine() {
        initEngineCalled = true;
        if (!_engine) {
            _engine = std::make_shared<gameEngine::GameEngine>();
        }
    }

    void initEngineRender() {
        initEngineRenderCalled = true;
    }

    void setIsServer(bool isServer) { _isServer = isServer; }
    bool isServer() const { return _isServer; }

    void processServerPackets(const std::vector<common::protocol::Packet>& packets, uint64_t elapsedMs) {
        lastServerPackets = packets;
        lastServerElapsed = elapsedMs;
    }

    void processClientPackets(const std::vector<common::protocol::Packet>& packets, uint64_t elapsedMs) {
        lastClientPackets = packets;
        lastClientElapsed = elapsedMs;
    }

    void buildServerPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t) {
        outgoingPackets = serverOutgoingPackets;
    }

    void buildClientPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t) {
        outgoingPackets = clientOutgoingPackets;
    }

    std::optional<common::protocol::Packet> spawnPlayerOnServer(uint32_t, float, float) {
        if (spawnPlayerFails) {
            return std::nullopt;
        }
        common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_SPAWN));
        packet.data.resize(ENTITY_SPAWN_PAYLOAD_SIZE, 0);
        return packet;
    }

    bool shouldSendPacketToPlayer(const common::protocol::Packet& packet, uint32_t targetPlayerId) const {
        if (packet.header.packet_type == static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_INPUT)) {
            return targetPlayerId != filteredPlayerId;
        }
        return true;
    }

    std::vector<uint32_t> getAllConnectedPlayerIds() const {
        return connectedPlayerIds;
    }

    std::shared_ptr<gameEngine::GameEngine> getEngine() { return _engine; }

    Entity createLevelEntity(int, float, const std::string&, const std::string&) {
        auto entity = _engine->createEntity("Level");
        _engine->addComponent<Level>(entity, Level{});
        return entity;
    }

    bool initEngineCalled = false;
    bool initEngineRenderCalled = false;
    bool spawnPlayerFails = false;
    bool _isServer = false;

    uint32_t filteredPlayerId = 0;
    std::vector<uint32_t> connectedPlayerIds;
    std::vector<common::protocol::Packet> serverOutgoingPackets;
    std::vector<common::protocol::Packet> clientOutgoingPackets;

    std::vector<common::protocol::Packet> lastServerPackets;
    std::vector<common::protocol::Packet> lastClientPackets;
    uint64_t lastServerElapsed = 0;
    uint64_t lastClientElapsed = 0;

private:
    std::shared_ptr<gameEngine::GameEngine> _engine;
};

#endif /* TESTS_STUB_COORDINATOR_HPP_ */
