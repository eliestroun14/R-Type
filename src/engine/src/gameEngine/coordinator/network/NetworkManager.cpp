/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** NetworkManager implementation
*/

#include "../../../../include/engine/gameEngine/coordinator/network/NetworkManager.hpp"
#include "../../../../../common/include/common/protocol/Packet.hpp"
#include <cstring>
#include "../../../../../engine/include/engine/utils/Logger.hpp"
#include "NetworkManager.hpp"

NetworkManager::PacketHandler *NetworkManager::findHandler(protocol::PacketTypes type)
{
    for (auto &handler : handlers) {
        if (handler.type == type) {
            return const_cast<PacketHandler*>(&handler);
        }
    }
    return nullptr;
}

std::optional<common::protocol::Packet> NetworkManager::processPacket(const common::protocol::Packet &packet)
{
    const auto &header = packet.header;

    // Check magic number
    if (header.magic != 0x5254)
        return std::nullopt;

    const auto packetType = static_cast<protocol::PacketTypes>(header.packet_type);

    // Find handler for this packet type
    for (const auto &handler : handlers) {
        if (handler.type == packetType) {
            if (handler.assertFunc(packet)) {
                return packet;
            }
            return std::nullopt;
        }
    }

    return std::nullopt;
}

common::protocol::Packet NetworkManager::createPacket(protocol::PacketTypes type, const std::vector<uint8_t> &args)
{
    // Find handler for this packet type
    for (const auto &handler : handlers) {
        if (handler.type == type) {
            return handler.createFunc(args);
        }
    }

    // Return empty packet if type not found
    return common::protocol::Packet();
}

// ==============================================================
//                  CONNECTION (0x01-0x0F)
// ==============================================================

bool NetworkManager::assertClientConnect(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;

    if (data.size() != 15) {
        LOG_ERROR_CAT("NetworkManager" ,"assertEntitySpawn: size of payload != 14 is = %zu", data.size());
        return false;
    }


    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertServerAccept(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertServerReject(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertClientDisconnect(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertHeartBeat(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

// ==============================================================
//                  INPUT (0x10-0x1F)
// ==============================================================

bool NetworkManager::assertPlayerInput(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

// ==============================================================
//                  WORLD_STATE (0x20-0x3F)
// ==============================================================

bool NetworkManager::assertWorldSnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;

    // WorldSnapshot: 18 bytes header + (entity_count * 16 bytes EntityState)
    if (data.size() < 18 || (data.size() - 18) % 16 != 0)
        return false;

    return true;
}

bool NetworkManager::assertEntitySpawn(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;

    if (data.size() != 15) {
        LOG_ERROR_CAT("NetworkManager" ,"assertEntitySpawn: size of payload != 15 is = %zu", data.size());
        return false;
    }

    // check if id is different than 0
    uint32_t entity_id;
    std::memcpy(&entity_id, data.data(), sizeof(uint32_t));
    if (entity_id == 0) {
        LOG_ERROR_CAT("NetworkManager" ,"assertEntitySpawn: id = 0");
        return false;
    }

    // check if entity_type exist
    uint8_t entity_type;
    std::memcpy(&entity_type, data.data() + 4, sizeof(uint8_t));
    if (entity_type < protocol::ENTITY_TYPE_MIN || entity_type > protocol::ENTITY_TYPE_MAX) {
        LOG_ERROR_CAT("NetworkManager" ,"assertEntitySpawn: entity_type does not exist: 0x%02hhx", entity_type);
        return false;
    }

    uint8_t initial_health;
    std::memcpy(&initial_health, data.data() + 10, sizeof(uint8_t));
    if (!(initial_health > 0)) {
        LOG_ERROR_CAT("NetworkManager" ,"assertEntitySpawn: initial_health <= 0: %hhu", initial_health);
        return false;
    }

    const auto &header = packet.header;

    if ((header.flags & 0x01) == 0) {  // 0x01 = FLAG_RELIABLE
        LOG_ERROR_CAT("NetworkManager", "Packet must have FLAG_RELIABLE");
        return false;
    }

    //TODO: need to check velocity, pos and variant

    return true;
}

bool NetworkManager::assertEntityDestroy(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertEntityUpdate(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertTransformSnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertVelocitySnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertHealthSnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertWeaponSnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertAISnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertAnimationSnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertComponentAdd(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertComponentRemove(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertTransformSnapshotDelta(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertHealthSnapshotDelta(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertEntityFullState(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

// ==============================================================
//                  GAME_EVENTS (0x40-0x5F)
// ==============================================================

bool NetworkManager::assertPlayerHit(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertPlayerDeath(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertScoreUpdate(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertPowerupPickup(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertWeaponFire(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertVisualEffect(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertAudioEffect(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertParticleSpawn(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

// ==============================================================
//                  GAME_CONTROL (0x60-0x6F)
// ==============================================================

bool NetworkManager::assertGameStart(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertGameEnd(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertLevelComplete(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertLevelStart(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertForceState(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertAIState(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

// ==============================================================
//                  PROTOCOL_CONTROL (0x70-0x7F)
// ==============================================================

bool NetworkManager::assertAcknowledgment(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertPing(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

bool NetworkManager::assertPong(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;
    return false;
}

// ==============================================================
//                  Creation Functions (Serialization)
// ==============================================================

// ==============================================================
//                  CONNECTION (0x01-0x0F)
// ==============================================================

common::protocol::Packet NetworkManager::createClientConnect(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_CONNECT));
    return packet;
}

common::protocol::Packet NetworkManager::createServerAccept(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_ACCEPT));
    return packet;
}

common::protocol::Packet NetworkManager::createServerReject(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_REJECT));
    return packet;
}

common::protocol::Packet NetworkManager::createClientDisconnect(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_DISCONNECT));
    return packet;
}

common::protocol::Packet NetworkManager::createHeartBeat(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEARTBEAT));
    return packet;
}

// ==============================================================
//                  INPUT (0x10-0x1F)
// ==============================================================

common::protocol::Packet NetworkManager::createPlayerInput(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_INPUT));
    return packet;
}

// ==============================================================
//                  WORLD_STATE (0x20-0x3F)
// ==============================================================

common::protocol::Packet NetworkManager::createWorldSnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_WORLD_SNAPSHOT));
    return packet;
}

common::protocol::Packet NetworkManager::createEntitySpawn(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_SPAWN));
    return packet;
}

common::protocol::Packet NetworkManager::createEntityDestroy(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_DESTROY));
    return packet;
}

common::protocol::Packet NetworkManager::createEntityUpdate(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_UPDATE));
    return packet;
}

common::protocol::Packet NetworkManager::createTransformSnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT));
    return packet;
}

common::protocol::Packet NetworkManager::createVelocitySnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_VELOCITY_SNAPSHOT));
    return packet;
}

common::protocol::Packet NetworkManager::createHealthSnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT));
    return packet;
}

common::protocol::Packet NetworkManager::createWeaponSnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_SNAPSHOT));
    return packet;
}

common::protocol::Packet NetworkManager::createAISnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_AI_SNAPSHOT));
    return packet;
}

common::protocol::Packet NetworkManager::createAnimationSnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ANIMATION_SNAPSHOT));
    return packet;
}

common::protocol::Packet NetworkManager::createComponentAdd(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_COMPONENT_ADD));
    return packet;
}

common::protocol::Packet NetworkManager::createComponentRemove(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_COMPONENT_REMOVE));
    return packet;
}

common::protocol::Packet NetworkManager::createTransformSnapshotDelta(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT_DELTA));
    return packet;
}

common::protocol::Packet NetworkManager::createHealthSnapshotDelta(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT_DELTA));
    return packet;
}

common::protocol::Packet NetworkManager::createEntityFullState(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_FULL_STATE));
    return packet;
}

// ==============================================================
//                  GAME_EVENTS (0x40-0x5F)
// ==============================================================

common::protocol::Packet NetworkManager::createPlayerHit(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_HIT));
    return packet;
}

common::protocol::Packet NetworkManager::createPlayerDeath(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_DEATH));
    return packet;
}

common::protocol::Packet NetworkManager::createScoreUpdate(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_SCORE_UPDATE));
    return packet;
}

common::protocol::Packet NetworkManager::createPowerupPickup(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_POWER_PICKUP));
    return packet;
}

common::protocol::Packet NetworkManager::createWeaponFire(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_FIRE));
    return packet;
}

common::protocol::Packet NetworkManager::createVisualEffect(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_VISUAL_EFFECT));
    return packet;
}

common::protocol::Packet NetworkManager::createAudioEffect(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_AUDIO_EFFECT));
    return packet;
}

common::protocol::Packet NetworkManager::createParticleSpawn(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PARTICLE_SPAWN));
    return packet;
}

// ==============================================================
//                  GAME_CONTROL (0x60-0x6F)
// ==============================================================

common::protocol::Packet NetworkManager::createGameStart(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_GAME_START));
    return packet;
}

common::protocol::Packet NetworkManager::createGameEnd(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_GAME_END));
    return packet;
}

common::protocol::Packet NetworkManager::createLevelComplete(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_LEVEL_COMPLETE));
    return packet;
}

common::protocol::Packet NetworkManager::createLevelStart(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_LEVEL_START));
    return packet;
}

common::protocol::Packet NetworkManager::createForceState(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_FORCE_STATE));
    return packet;
}

common::protocol::Packet NetworkManager::createAIState(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_AI_STATE));
    return packet;
}

// ==============================================================
//                  PROTOCOL_CONTROL (0x70-0x7F)
// ==============================================================

common::protocol::Packet NetworkManager::createAcknowledgment(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ACK));
    return packet;
}

common::protocol::Packet NetworkManager::createPing(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PING));
    return packet;
}

common::protocol::Packet NetworkManager::createPong(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PONG));
    return packet;
}

