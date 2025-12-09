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

bool NetworkManager::assertEntitySpawn(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;

    if (data.size() != 15) {
        LOG_ERROR_CAT("NetworkManager" ,"assertEntitySpawn: size of payload != 14 is = %zu", data.size());
        return false;
    }

    // check if if id is diferent than 0
    uint32_t entity_id;
    std::memcpy(&entity_id, data.data(), sizeof(uint32_t));
    if (entity_id == 0) {
        LOG_ERROR_CAT("NetworkManager" ,"assertEntitySpawn: id = 0");
        return false;
    }

    // check if entity_type exist
    uint8_t entity_type = data[4];
    if (entity_type < protocol::ENTITY_TYPE_MIN || entity_type > protocol::ENTITY_TYPE_MAX) {
        LOG_ERROR_CAT("NetworkManager" ,"assertEntitySpawn: entity_type does not exist: 0x%02hhx", entity_type);
        return false;
    }

    uint8_t initial_health = data[10];
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

bool NetworkManager::assertWorldSnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    
    // WorldSnapshot: 18 bytes header + (entity_count * 16 bytes EntityState)
    if (data.size() < 18 || (data.size() - 18) % 16 != 0)
        return false;

    return true;
}

common::protocol::Packet NetworkManager::createEntitySpawn(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_SPAWN));
    // TODO: Build payload from args
    return packet;
}

common::protocol::Packet NetworkManager::createWorldSnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_WORLD_SNAPSHOT));
    // TODO: Build payload from args
    return packet;
}

