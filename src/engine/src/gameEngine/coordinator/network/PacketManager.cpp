/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PacketManager implementation
*/

#include "../../../../include/engine/gameEngine/coordinator/network/PacketManager.hpp"
#include "../../../../../common/include/common/protocol/Packet.hpp"
#include <cstring>
#include "../../../../../engine/include/engine/utils/Logger.hpp"

PacketManager::PacketHandler *PacketManager::findHandler(protocol::PacketTypes type)
{
    for (auto &handler : handlers) {
        if (handler.type == type) {
            return const_cast<PacketHandler*>(&handler);
        }
    }
    return nullptr;
}

std::optional<common::protocol::Packet> PacketManager::processPacket(const common::protocol::Packet &packet)
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

common::protocol::Packet PacketManager::createPacket(protocol::PacketTypes type, const std::vector<uint8_t> &args)
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
//                  HELPER FUNCTIONS
// ==============================================================

/**
 * @brief Validates an EntityState structure (15 bytes)
 * @param data Buffer containing EntityState
 * @param offset Offset in buffer where EntityState starts
 * @param entity_index Index for logging (optional, set to -1 to omit from logs)
 * @return true if EntityState is valid, false otherwise
 */
static bool validateEntityState(const std::vector<uint8_t> &data, size_t offset, int entity_index = -1)
{
    // Check if we have enough bytes for EntityState
    if (offset + 15 > data.size()) {
        if (entity_index >= 0) {
            LOG_ERROR_CAT("PacketManager", "validateEntityState[%d]: not enough data, need 15 bytes", entity_index);
        } else {
            LOG_ERROR_CAT("PacketManager", "validateEntityState: not enough data, need 15 bytes");
        }
        return false;
    }

    // Offset +0 to +3: entity_id must not be 0
    uint32_t entity_id;
    std::memcpy(&entity_id, data.data() + offset + 0, sizeof(uint32_t));
    if (entity_id == 0) {
        if (entity_index >= 0) {
            LOG_ERROR_CAT("PacketManager", "validateEntityState[%d]: entity_id == 0", entity_index);
        } else {
            LOG_ERROR_CAT("PacketManager", "validateEntityState: entity_id == 0");
        }
        return false;
    }

    // Offset +4: entity_type must be valid (0x01 to 0x08)
    uint8_t entity_type;
    std::memcpy(&entity_type, data.data() + offset + 4, sizeof(uint8_t));
    if (entity_type < protocol::ENTITY_TYPE_MIN || entity_type > protocol::ENTITY_TYPE_MAX) {
        if (entity_index >= 0) {
            LOG_ERROR_CAT("PacketManager", "validateEntityState[%d]: entity_type invalid 0x%02hhx", entity_index, entity_type);
        } else {
            LOG_ERROR_CAT("PacketManager", "validateEntityState: entity_type invalid 0x%02hhx", entity_type);
        }
        return false;
    }

    // Offset +5 to +6: position_x (uint16_t)
    // Offset +7 to +8: position_y (uint16_t)
    // World coordinates typically 0-65535, automatically valid for uint16_t
    // TODO: check if we know the size of the world map

    // Offset +9 to +10: velocity_x (int16_t)
    // Offset +11 to +12: velocity_y (int16_t)
    // Velocity can be any int16_t value
    // TODO: check if we know the max value possible of velocity

    // Offset +13: health can be 0 if entity is dead, or > 0 if alive
    uint8_t health;
    std::memcpy(&health, data.data() + offset + 13, sizeof(uint8_t));
    // health can be 0 (entity dead) or > 0 (entity alive)

    // Offset +14: state_flags (uint8_t) - any value is technically valid

    return true;
}

// ==============================================================
//                  CONNECTION (0x01-0x0F)
// ==============================================================

bool PacketManager::assertClientConnect(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 37 bytes
    if (data.size() != 37) {
        LOG_ERROR_CAT("PacketManager", "assertClientConnect: payload size != 37, got %zu", data.size());
        return false;
    }

    // Offset 0: protocol_version must be 1
    uint8_t protocol_version;
    std::memcpy(&protocol_version, data.data() + 0, sizeof(uint8_t));
    if (protocol_version != 1) {
        LOG_ERROR_CAT("PacketManager", "assertClientConnect: protocol_version != 1, got %hhu", protocol_version);
        return false;
    }

    // Offset 1-32: player_name (char[32]) - must be non-empty and not all zeros
    const char *player_name = reinterpret_cast<const char *>(data.data() + 1);
    bool is_empty = true;
    for (int i = 0; i < 31; ++i) {  // 31 chars max (32nd is null terminator)
        if (player_name[i] != '\0' && player_name[i] != 0) {
            is_empty = false;
            break;
        }
    }
    if (is_empty) {
        LOG_ERROR_CAT("PacketManager", "assertClientConnect: player_name is empty");
        return false;
    }

    // Offset 33-36: client_id must not be 0
    uint32_t client_id;
    std::memcpy(&client_id, data.data() + 33, sizeof(uint32_t));
    if (client_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertClientConnect: client_id == 0");
        return false;
    }

    return true;
}

bool PacketManager::assertServerAccept(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 11 bytes
    if (data.size() != 11) {
        LOG_ERROR_CAT("PacketManager", "assertServerAccept: payload size != 11, got %zu", data.size());
        return false;
    }

    // Offset 0-3: assigned_player_id must not be 0
    uint32_t assigned_player_id;
    std::memcpy(&assigned_player_id, data.data() + 0, sizeof(uint32_t));
    if (assigned_player_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertServerAccept: assigned_player_id == 0");
        return false;
    }

    // Offset 4: max_players must be between 1 and 4
    uint8_t max_players;
    std::memcpy(&max_players, data.data() + 4, sizeof(uint8_t));
    if (max_players < 1 || max_players > 4) {
        LOG_ERROR_CAT("PacketManager", "assertServerAccept: max_players not in [1,4], got %hhu", max_players);
        return false;
    }

    // Offset 5-8: game_instance_id must not be 0
    uint32_t game_instance_id;
    std::memcpy(&game_instance_id, data.data() + 5, sizeof(uint32_t));
    if (game_instance_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertServerAccept: game_instance_id == 0");
        return false;
    }

    // Offset 9-10: server_tickrate

    return true;
}

bool PacketManager::assertServerReject(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 65 bytes
    if (data.size() != 65) {
        LOG_ERROR_CAT("PacketManager", "assertServerReject: payload size != 65, got %zu", data.size());
        return false;
    }

    // Offset 0: reject_code must be valid
    uint8_t reject_code;
    std::memcpy(&reject_code, data.data() + 0, sizeof(uint8_t));
    bool valid_code = (reject_code >= protocol::REJECT_CODE_MIN && reject_code <= protocol::REJECT_CODE_MAX) ||
        reject_code == protocol::REJECT_CODE_GENERIC_ERROR;
    if (!valid_code) {
        LOG_ERROR_CAT("PacketManager", "assertServerReject: invalid reject_code 0x%02hhx", reject_code);
        return false;
    }

    // Offset 1-64: reason_message (char[64]) - must not be empty
    const char *reason_message = reinterpret_cast<const char *>(data.data() + 1);
    bool is_empty = true;
    for (int i = 0; i < 63; ++i) {  // 63 chars max (64th is null terminator)
        if (reason_message[i] != '\0' && reason_message[i] != 0) {
            is_empty = false;
            break;
        }
    }
    if (is_empty) {
        LOG_ERROR_CAT("PacketManager", "assertServerReject: reason_message is empty");
        return false;
    }

    return true;
}

bool PacketManager::assertClientDisconnect(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 5 bytes
    if (data.size() != 5) {
        LOG_ERROR_CAT("PacketManager", "assertClientDisconnect: payload size != 5, got %zu", data.size());
        return false;
    }

    // Offset 0-3: player_id must not be 0
    uint32_t player_id;
    std::memcpy(&player_id, data.data() + 0, sizeof(uint32_t));
    if (player_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertClientDisconnect: player_id == 0");
        return false;
    }

    // Offset 4: reason must be valid DisconnectReasons
    uint8_t reason;
    std::memcpy(&reason, data.data() + 4, sizeof(uint8_t));
    bool valid_reason = (reason >= protocol::DISCONNECT_REASON_MIN && reason <= protocol::DISCONNECT_REASON_MAX) ||
        reason == protocol::DISCONNECT_REASON_GENERIC_ERROR;
    if (!valid_reason) {
        LOG_ERROR_CAT("PacketManager", "assertClientDisconnect: invalid reason 0x%02hhx", reason);
        return false;
    }

    // Must have FLAG_RELIABLE
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertClientDisconnect: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

bool PacketManager::assertHeartBeat(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 4 bytes
    if (data.size() != 4) {
        LOG_ERROR_CAT("PacketManager", "assertHeartBeat: payload size != 4, got %zu", data.size());
        return false;
    }

    // Offset 0-3: player_id must not be 0
    uint32_t player_id;
    std::memcpy(&player_id, data.data() + 0, sizeof(uint32_t));
    if (player_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertHeartBeat: player_id == 0");
        return false;
    }

    return true;
}

// ==============================================================
//                  INPUT (0x10-0x1F)
// ==============================================================

bool PacketManager::assertPlayerInput(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 12 bytes
    if (data.size() != 12) {
        LOG_ERROR_CAT("PacketManager", "assertPlayerInput: payload size != 12, got %zu", data.size());
        return false;
    }

    // Offset 0-3: player_id must not be 0
    uint32_t player_id;
    std::memcpy(&player_id, data.data() + 0, sizeof(uint32_t));
    if (player_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertPlayerInput: player_id == 0");
        return false;
    }

    // Offset 4-5: input_state must be valid combination of InputFlags (bits 0-8)
    uint16_t input_state;
    std::memcpy(&input_state, data.data() + 4, sizeof(uint16_t));
    if ((input_state & ~protocol::INPUT_FLAGS_MASK) != 0) {  // Only bits 0-8 are valid
        LOG_ERROR_CAT("PacketManager", "assertPlayerInput: input_state has invalid bits 0x%04hx", input_state);
        return false;
    }

    // Offset 6-7, 8-9: aim_direction_x and aim_direction_y should form normalized vector or (0,0)
    // For now, just ensure they're reasonable values
    // TODO: Validate that sqrt(x² + y²) ≈ 1.0 (tolerance ±0.1) or (0,0)

    return true;
}

// ==============================================================
//                  WORLD_STATE (0x20-0x3F)
// ==============================================================

bool PacketManager::assertWorldSnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;

    // WorldSnapshot payload: 6 bytes header + (entity_count * 15 bytes EntityState)
    if (data.size() < 6 || (data.size() - 6) % 15 != 0) {
        LOG_ERROR_CAT("PacketManager", "assertWorldSnapshot: invalid payload size %zu", data.size());
        return false;
    }

    // Offset 0-3: world_tick (no strict validation, but should be monotonic increasing in practice)
    uint32_t world_tick;
    std::memcpy(&world_tick, data.data() + 0, sizeof(uint32_t));
    // TODO: Track previous world_tick and validate monotonic increase

    // Offset 4-5: entity_count (uint16_t - always >= 0)
    uint16_t entity_count;
    std::memcpy(&entity_count, data.data() + 4, sizeof(uint16_t));

    // Validate total size matches entity count
    if (data.size() != 6 + (entity_count * 15)) {
        LOG_ERROR_CAT("PacketManager", "assertWorldSnapshot: size mismatch, expected %zu got %zu",
                     6 + (entity_count * 15), data.size());
        return false;
    }

    // Validate each EntityState starting at offset 6
    for (uint16_t i = 0; i < entity_count; ++i) {
        size_t entity_offset = 6 + (i * 15);

        if (!validateEntityState(data, entity_offset, i)) {
            return false;
        }
    }

    return true;
}

bool PacketManager::assertEntitySpawn(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 15 bytes (EntityState)
    if (data.size() != 15) {
        LOG_ERROR_CAT("PacketManager", "assertEntitySpawn: payload size != 15, got %zu", data.size());
        return false;
    }

    // Validate EntityState structure
    if (!validateEntityState(data, 0)) {
        return false;
    }

    // For spawning, health must be > 0 (cannot spawn dead entity)
    uint8_t health;
    std::memcpy(&health, data.data() + 13, sizeof(uint8_t));
    if (health == 0) {
        LOG_ERROR_CAT("PacketManager", "assertEntitySpawn: cannot spawn entity with health == 0");
        return false;
    }

    // Must have FLAG_RELIABLE
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertEntitySpawn: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}


bool PacketManager::assertEntityDestroy(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 9 bytes
    if (data.size() != 9) {
        LOG_ERROR_CAT("PacketManager", "assertEntityDestroy: payload size != 9, got %zu", data.size());
        return false;
    }

    // Offset 0-3: entity_id must not be 0
    uint32_t entity_id;
    std::memcpy(&entity_id, data.data() + 0, sizeof(uint32_t));
    if (entity_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertEntityDestroy: entity_id == 0");
        return false;
    }

    // Offset 4: destroy_reason must be valid EntityDestroyReasons
    uint8_t destroy_reason;
    std::memcpy(&destroy_reason, data.data() + 4, sizeof(uint8_t));
    if (destroy_reason < protocol::ENTITY_DESTROY_REASON_MIN || destroy_reason > protocol::ENTITY_DESTROY_REASON_MAX) {
        LOG_ERROR_CAT("PacketManager", "assertEntityDestroy: invalid destroy_reason 0x%02hhx", destroy_reason);
        return false;
    }

    // Offset 5-6, 7-8: final_position_x and final_position_y should be valid coordinates
    // TODO: check if have the position and the value of the map

    // Must have FLAG_RELIABLE
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertEntityDestroy: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

bool PacketManager::assertEntityUpdate(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 16 bytes
    if (data.size() != 16) {
        LOG_ERROR_CAT("PacketManager", "assertEntityUpdate: payload size != 16, got %zu", data.size());
        return false;
    }

    // Offset 0-3: entity_id must not be 0
    uint32_t entity_id;
    std::memcpy(&entity_id, data.data() + 0, sizeof(uint32_t));
    if (entity_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertEntityUpdate: entity_id == 0");
        return false;
    }

    // Offset 4: update_flags must be valid combination of EntityUpdateFlags
    uint8_t update_flags;
    std::memcpy(&update_flags, data.data() + 4, sizeof(uint8_t));
    if ((update_flags & ~protocol::ENTITY_UPDATE_FLAGS_MASK) != 0) {  // Only bits 0-5 are valid
        LOG_ERROR_CAT("PacketManager", "assertEntityUpdate: update_flags has invalid bits 0x%02hhx", update_flags);
        return false;
    }

    // Offset 5-6, 7-8: pos_x, pos_y (only validated if UPDATE_POS flag set)
    // Offset 9: health (only validated if UPDATE_HEALTH flag set)
    // Offset 10: shield (only validated if UPDATE_SHIELD flag set)
    // Offset 11: state_flags (only validated if UPDATE_STATE_FLAGS flag set)
    // Offset 12-13, 14-15: velocity_x, velocity_y (only validated if UPDATE_VELOCITY flag set)
    // TODO: is it an error to have a value but not the flag set

    return true;
}

// ==============================================================
//             ECS COMPONENTS SNAPSHOTS (0x24-0x2F)
// ==============================================================

bool PacketManager::assertTransformSnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // TransformSnapshot: 6 + (entity_count × 12) bytes minimum 6 bytes
    if (data.size() < 6 || (data.size() - 6) % 12 != 0) {
        LOG_ERROR_CAT("PacketManager", "assertTransformSnapshot: invalid payload size %zu", data.size());
        return false;
    }

    // Offset 0-3: world_tick
    uint32_t world_tick;
    std::memcpy(&world_tick, data.data() + 0, sizeof(uint32_t));
    // TODO: Check monotonicity with previous ticks

    // Offset 4-5: entity_count
    uint16_t entity_count;
    std::memcpy(&entity_count, data.data() + 4, sizeof(uint16_t));
    if (entity_count < 0) {
        LOG_ERROR_CAT("PacketManager", "assertTransformSnapshot: entity_count < 0 got: %hu", entity_count);
        return false;
    }

    // Validate size matches entity count
    if (data.size() != 6 + (entity_count * 12)) {
        LOG_ERROR_CAT("PacketManager", "assertTransformSnapshot: size mismatch, got %zu expected %zu",
                     data.size(), 6 + (entity_count * 12));
        return false;
    }

    return true;
}

bool PacketManager::assertVelocitySnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // VelocitySnapshot: 6 + (entity_count × 12) bytes
    if (data.size() < 6 || (data.size() - 6) % 12 != 0) {
        LOG_ERROR_CAT("PacketManager", "assertVelocitySnapshot: invalid payload size %zu", data.size());
        return false;
    }

    // Offset 0-3: world_tick - should be monotonic increasing
    uint32_t world_tick;
    std::memcpy(&world_tick, data.data() + 0, sizeof(uint32_t));
    // TODO: Track previous world_tick and validate monotonic increase

    // Offset 4-5: entity_count - must be valid
    uint16_t entity_count;
    std::memcpy(&entity_count, data.data() + 4, sizeof(uint16_t));
    // entity_count is uint16_t, always >= 0

    // Validate size matches entity count
    if (data.size() != 6 + (entity_count * 12)) {
        LOG_ERROR_CAT("PacketManager", "assertVelocitySnapshot: size mismatch, got %zu expected %zu",
                     data.size(), 6 + (entity_count * 12));
        return false;
    }

    // Validate each ComponentVelocity entry (12 bytes each: entity_id + 8 bytes data)
    for (uint16_t i = 0; i < entity_count; ++i) {
        size_t entry_offset = 6 + (i * 12);

        // Offset +0 to +3: entity_id (should not be 0, but not strictly enforced here)
        uint32_t entity_id;
        std::memcpy(&entity_id, data.data() + entry_offset + 0, sizeof(uint32_t));
        if (entity_id == 0) {
            LOG_ERROR_CAT("PacketManager", "assertVelocitySnapshot: entry[%hu].entity_id == 0", i);
            return false;
        }

        // Offset +4 to +5: vel_x (int16_t)
        // Offset +6 to +7: vel_y (int16_t)
        // TODO: Validate velocity magnitude is reasonable (< MAX_VELOCITY)
        // TODO: Calculate sqrt(vel_x² + vel_y²) and ensure it's within limits

        // Offset +8 to +9: acceleration_x (int16_t)
        // Offset +10 to +11: acceleration_y (int16_t)
        // TODO: Validate acceleration magnitude is reasonable (< MAX_ACCELERATION)
    }

    return true;
}

bool PacketManager::assertHealthSnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // HealthSnapshot: 6 + (entity_count × 8) bytes
    if (data.size() < 6 || (data.size() - 6) % 8 != 0) {
        LOG_ERROR_CAT("PacketManager", "assertHealthSnapshot: invalid payload size %zu", data.size());
        return false;
    }

    // Offset 0-3: world_tick - should be monotonic increasing
    uint32_t world_tick;
    std::memcpy(&world_tick, data.data() + 0, sizeof(uint32_t));
    // TODO: Track previous world_tick and validate monotonic increase

    // Offset 4-5: entity_count - must be valid
    uint16_t entity_count;
    std::memcpy(&entity_count, data.data() + 4, sizeof(uint16_t));
    // entity_count is uint16_t, always >= 0

    // Validate size matches entity count
    if (data.size() != 6 + (entity_count * 8)) {
        LOG_ERROR_CAT("PacketManager", "assertHealthSnapshot: size mismatch, got %zu expected %zu",
                     data.size(), 6 + (entity_count * 8));
        return false;
    }

    // Validate each ComponentHealth entry (8 bytes each)
    // Per entry: [uint32_t entity_id (4 bytes)] + [ComponentHealth: current_health(1) + max_health(1) + current_shield(1) + max_shield(1)]
    for (uint16_t i = 0; i < entity_count; ++i) {
        size_t entry_offset = 6 + (i * 8);

        // Offset +0 to +3: entity_id (should not be 0)
        uint32_t entity_id;
        std::memcpy(&entity_id, data.data() + entry_offset + 0, sizeof(uint32_t));
        if (entity_id == 0) {
            LOG_ERROR_CAT("PacketManager", "assertHealthSnapshot: entry[%hu].entity_id == 0", i);
            return false;
        }

        // Offset +4: current_health (uint8_t) - can be 0 if entity is dead
        uint8_t current_health;
        std::memcpy(&current_health, data.data() + entry_offset + 4, sizeof(uint8_t));
        // current_health can be 0 (entity dead) or > 0 (entity alive)

        // Offset +5: max_health (uint8_t) - must be > 0
        uint8_t max_health;
        std::memcpy(&max_health, data.data() + entry_offset + 5, sizeof(uint8_t));
        if (max_health == 0) {
            LOG_ERROR_CAT("PacketManager", "assertHealthSnapshot: entry[%hu].max_health == 0", i);
            return false;
        }

        // Offset +6: current_shield (uint8_t) - can be 0
        // Offset +7: max_shield (uint8_t) - can be 0

        // TODO: Validate current_health <= max_health
        // TODO: Validate current_shield <= max_shield
    }

    return true;
}

bool PacketManager::assertWeaponSnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // WeaponSnapshot: 6 + (entity_count × 9) bytes
    if (data.size() < 6 || (data.size() - 6) % 9 != 0) {
        LOG_ERROR_CAT("PacketManager", "assertWeaponSnapshot: invalid payload size %zu", data.size());
        return false;
    }

    // Offset 0-3: world_tick
    uint32_t world_tick;
    std::memcpy(&world_tick, data.data() + 0, sizeof(uint32_t));

    // Offset 4-5: entity_count
    uint16_t entity_count;
    std::memcpy(&entity_count, data.data() + 4, sizeof(uint16_t));

    // Validate size matches entity count
    if (data.size() != 6 + (entity_count * 9)) {
        LOG_ERROR_CAT("PacketManager", "assertWeaponSnapshot: size mismatch, got %zu expected %zu",
                     data.size(), 6 + (entity_count * 9));
        return false;
    }

    return true;
}

bool PacketManager::assertAISnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // AISnapshot: 6 + (entity_count × 12) bytes
    if (data.size() < 6 || (data.size() - 6) % 12 != 0) {
        LOG_ERROR_CAT("PacketManager", "assertAISnapshot: invalid payload size %zu", data.size());
        return false;
    }

    // Offset 0-3: world_tick
    uint32_t world_tick;
    std::memcpy(&world_tick, data.data() + 0, sizeof(uint32_t));

    // Offset 4-5: entity_count
    uint16_t entity_count;
    std::memcpy(&entity_count, data.data() + 4, sizeof(uint16_t));

    // Validate size matches entity count
    if (data.size() != 6 + (entity_count * 12)) {
        LOG_ERROR_CAT("PacketManager", "assertAISnapshot: size mismatch, got %zu expected %zu",
                     data.size(), 6 + (entity_count * 12));
        return false;
    }

    return true;
}

bool PacketManager::assertAnimationSnapshot(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // AnimationSnapshot: 6 + (entity_count × 11) bytes
    if (data.size() < 6 || (data.size() - 6) % 11 != 0) {
        LOG_ERROR_CAT("PacketManager", "assertAnimationSnapshot: invalid payload size %zu", data.size());
        return false;
    }

    // Offset 0-3: world_tick
    uint32_t world_tick;
    std::memcpy(&world_tick, data.data() + 0, sizeof(uint32_t));

    // Offset 4-5: entity_count
    uint16_t entity_count;
    std::memcpy(&entity_count, data.data() + 4, sizeof(uint16_t));

    // Validate size matches entity count
    if (data.size() != 6 + (entity_count * 11)) {
        LOG_ERROR_CAT("PacketManager", "assertAnimationSnapshot: size mismatch, got %zu expected %zu",
                     data.size(), 6 + (entity_count * 11));
        return false;
    }

    return true;
}

bool PacketManager::assertComponentAdd(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // ComponentAdd: 6 + data_size bytes minimum
    if (data.size() < 6) {
        LOG_ERROR_CAT("PacketManager", "assertComponentAdd: payload size < 6, got %zu", data.size());
        return false;
    }

    // Offset 0-3: entity_id must not be 0
    uint32_t entity_id;
    std::memcpy(&entity_id, data.data() + 0, sizeof(uint32_t));
    if (entity_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertComponentAdd: entity_id == 0");
        return false;
    }

    // Offset 4: component_type must be valid (0x01 to 0x0F)
    uint8_t component_type;
    std::memcpy(&component_type, data.data() + 4, sizeof(uint8_t));
    if (component_type < 1 || component_type > 15) {
        LOG_ERROR_CAT("PacketManager", "assertComponentAdd: invalid component_type 0x%02hhx", component_type);
        return false;
    }

    // Offset 5: data_size
    uint8_t data_size;
    std::memcpy(&data_size, data.data() + 5, sizeof(uint8_t));

    // Validate total size
    if (data.size() != 6 + data_size) {
        LOG_ERROR_CAT("PacketManager", "assertComponentAdd: size mismatch, got %zu expected %zu",
                     data.size(), 6 + data_size);
        return false;
    }

    // TODO: Validate component_data based on component_type
    // TRANSFORM: 8 bytes, VELOCITY: 8 bytes, HEALTH: 4 bytes, etc.

    // Must have FLAG_RELIABLE
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertComponentAdd: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

bool PacketManager::assertComponentRemove(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 5 bytes
    if (data.size() != 5) {
        LOG_ERROR_CAT("PacketManager", "assertComponentRemove: payload size != 5, got %zu", data.size());
        return false;
    }

    // Offset 0-3: entity_id must not be 0
    uint32_t entity_id;
    std::memcpy(&entity_id, data.data() + 0, sizeof(uint32_t));
    if (entity_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertComponentRemove: entity_id == 0");
        return false;
    }

    // Offset 4: component_type must be valid (0x01 to 0x0F)
    uint8_t component_type;
    std::memcpy(&component_type, data.data() + 4, sizeof(uint8_t));
    if (component_type < 1 || component_type > 15) {
        LOG_ERROR_CAT("PacketManager", "assertComponentRemove: invalid component_type 0x%02hhx", component_type);
        return false;
    }

    // Must have FLAG_RELIABLE
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertComponentRemove: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

bool PacketManager::assertTransformSnapshotDelta(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // TransformSnapshotDelta: 10 + (entity_count × 12) bytes
    if (data.size() < 10 || (data.size() - 10) % 12 != 0) {
        LOG_ERROR_CAT("PacketManager", "assertTransformSnapshotDelta: invalid payload size %zu", data.size());
        return false;
    }

    // Offset 0-3: world_tick
    uint32_t world_tick;
    std::memcpy(&world_tick, data.data() + 0, sizeof(uint32_t));

    // Offset 4-7: base_tick must be less than world_tick
    uint32_t base_tick;
    std::memcpy(&base_tick, data.data() + 4, sizeof(uint32_t));
    if (base_tick >= world_tick) {
        LOG_ERROR_CAT("PacketManager", "assertTransformSnapshotDelta: base_tick >= world_tick");
        return false;
    }

    // Offset 8-9: entity_count
    uint16_t entity_count;
    std::memcpy(&entity_count, data.data() + 8, sizeof(uint16_t));

    // Validate size matches entity count
    if (data.size() != 10 + (entity_count * 12)) {
        LOG_ERROR_CAT("PacketManager", "assertTransformSnapshotDelta: size mismatch, got %zu expected %zu",
                     data.size(), 10 + (entity_count * 12));
        return false;
    }

    return true;
}

bool PacketManager::assertHealthSnapshotDelta(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // HealthSnapshotDelta: 10 + (entity_count × 8) bytes
    if (data.size() < 10 || (data.size() - 10) % 8 != 0) {
        LOG_ERROR_CAT("PacketManager", "assertHealthSnapshotDelta: invalid payload size %zu", data.size());
        return false;
    }

    // Offset 0-3: world_tick
    uint32_t world_tick;
    std::memcpy(&world_tick, data.data() + 0, sizeof(uint32_t));

    // Offset 4-7: base_tick must be less than world_tick
    uint32_t base_tick;
    std::memcpy(&base_tick, data.data() + 4, sizeof(uint32_t));
    if (base_tick >= world_tick) {
        LOG_ERROR_CAT("PacketManager", "assertHealthSnapshotDelta: base_tick >= world_tick");
        return false;
    }

    // Offset 8-9: entity_count
    uint16_t entity_count;
    std::memcpy(&entity_count, data.data() + 8, sizeof(uint16_t));

    // Validate size matches entity count
    if (data.size() != 10 + (entity_count * 8)) {
        LOG_ERROR_CAT("PacketManager", "assertHealthSnapshotDelta: size mismatch, got %zu expected %zu",
                     data.size(), 10 + (entity_count * 8));
        return false;
    }

    return true;
}

bool PacketManager::assertEntityFullState(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // EntityFullState: 6 + variable component data
    if (data.size() < 6) {
        LOG_ERROR_CAT("PacketManager", "assertEntityFullState: payload size < 6, got %zu", data.size());
        return false;
    }

    // Offset 0-3: entity_id must not be 0
    uint32_t entity_id;
    std::memcpy(&entity_id, data.data() + 0, sizeof(uint32_t));
    if (entity_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertEntityFullState: entity_id == 0");
        return false;
    }

    // Offset 4: entity_type must be valid (0x01 to 0x08)
    uint8_t entity_type;
    std::memcpy(&entity_type, data.data() + 4, sizeof(uint8_t));
    if (entity_type < 1 || entity_type > 8) {
        LOG_ERROR_CAT("PacketManager", "assertEntityFullState: invalid entity_type 0x%02hhx", entity_type);
        return false;
    }

    // Offset 5: component_count must be > 0 and <= 15
    uint8_t component_count;
    std::memcpy(&component_count, data.data() + 5, sizeof(uint8_t));
    if (component_count == 0 || component_count > 15) {
        LOG_ERROR_CAT("PacketManager", "assertEntityFullState: component_count invalid %hhu", component_count);
        return false;
    }

    // TODO: Validate each component has valid type and size

    // Must have FLAG_RELIABLE
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertEntityFullState: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

// ==============================================================
//                  GAME_EVENTS (0x40-0x5F)
// ==============================================================

bool PacketManager::assertPlayerHit(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 17 bytes
    if (data.size() != 17) {
        LOG_ERROR_CAT("PacketManager", "assertPlayerHit: payload size != 17, got %zu", data.size());
        return false;
    }

    // Offset 0-3: player_id must not be 0
    uint32_t player_id;
    std::memcpy(&player_id, data.data() + 0, sizeof(uint32_t));
    if (player_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertPlayerHit: player_id == 0");
        return false;
    }

    // Offset 4-7: attacker_id must not be 0
    uint32_t attacker_id;
    std::memcpy(&attacker_id, data.data() + 4, sizeof(uint32_t));
    if (attacker_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertPlayerHit: attacker_id == 0");
        return false;
    }

    // Offset 8: damage must be > 0
    uint8_t damage;
    std::memcpy(&damage, data.data() + 8, sizeof(uint8_t));
    if (damage == 0) {
        LOG_ERROR_CAT("PacketManager", "assertPlayerHit: damage == 0");
        return false;
    }

    // Offset 9: remaining_health must be >= 0
    // Offset 10: remaining_shield must be >= 0
    // Skip detailed validation for now

    // Must have FLAG_RELIABLE (PlayerHit type = 0x40, FLAG_RELIABLE)
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertPlayerHit: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

bool PacketManager::assertPlayerDeath(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 18 bytes
    if (data.size() != 18) {
        LOG_ERROR_CAT("PacketManager", "assertPlayerDeath: payload size != 18, got %zu", data.size());
        return false;
    }

    // Offset 0-3: player_id must not be 0
    uint32_t player_id;
    std::memcpy(&player_id, data.data() + 0, sizeof(uint32_t));
    if (player_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertPlayerDeath: player_id == 0");
        return false;
    }

    // Offset 4-7: killer_id must not be 0
    uint32_t killer_id;
    std::memcpy(&killer_id, data.data() + 4, sizeof(uint32_t));
    if (killer_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertPlayerDeath: killer_id == 0");
        return false;
    }

    // Offset 8-11: score_before_death must be >= 0
    uint32_t score_before_death;
    std::memcpy(&score_before_death, data.data() + 8, sizeof(uint32_t));

    // Must have FLAG_RELIABLE (PlayerDeath type = 0x41, FLAG_RELIABLE)
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertPlayerDeath: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

bool PacketManager::assertScoreUpdate(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 11 bytes
    if (data.size() != 11) {
        LOG_ERROR_CAT("PacketManager", "assertScoreUpdate: payload size != 11, got %zu", data.size());
        return false;
    }

    // Offset 0-3: player_id must not be 0
    uint32_t player_id;
    std::memcpy(&player_id, data.data() + 0, sizeof(uint32_t));
    if (player_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertScoreUpdate: player_id == 0");
        return false;
    }

    // Offset 4-7: new_score must be >= 0
    uint32_t new_score;
    std::memcpy(&new_score, data.data() + 4, sizeof(uint32_t));

    // Offset 8-9: score_delta must not be 0
    int16_t score_delta;
    std::memcpy(&score_delta, data.data() + 8, sizeof(int16_t));
    if (score_delta == 0) {
        LOG_ERROR_CAT("PacketManager", "assertScoreUpdate: score_delta == 0");
        return false;
    }

    // Offset 10: reason must be valid ScoreChangeReasons (0x00 to 0x05)
    uint8_t reason;
    std::memcpy(&reason, data.data() + 10, sizeof(uint8_t));
    if (reason > 5) {
        LOG_ERROR_CAT("PacketManager", "assertScoreUpdate: invalid reason 0x%02hhx", reason);
        return false;
    }

    // TODO: Validate score_delta sign matches reason (positive reasons vs negative)

    // ScoreUpdate (type = 0x42) does NOT require FLAG_RELIABLE

    return true;
}

bool PacketManager::assertPowerupPickup(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 10 bytes
    if (data.size() != 10) {
        LOG_ERROR_CAT("PacketManager", "assertPowerupPickup: payload size != 10, got %zu", data.size());
        return false;
    }

    // Offset 0-3: player_id must not be 0
    uint32_t player_id;
    std::memcpy(&player_id, data.data() + 0, sizeof(uint32_t));
    if (player_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertPowerupPickup: player_id == 0");
        return false;
    }

    // Offset 4-7: powerup_id must not be 0
    uint32_t powerup_id;
    std::memcpy(&powerup_id, data.data() + 4, sizeof(uint32_t));
    if (powerup_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertPowerupPickup: powerup_id == 0");
        return false;
    }

    // Offset 8: powerup_type must be valid (0x00 to 0x05)
    uint8_t powerup_type;
    std::memcpy(&powerup_type, data.data() + 8, sizeof(uint8_t));
    if (powerup_type > 5) {
        LOG_ERROR_CAT("PacketManager", "assertPowerupPickup: invalid powerup_type 0x%02hhx", powerup_type);
        return false;
    }

    // Offset 9: duration must be >= 0
    // 0 = permanent is valid for certain types

    // Must have FLAG_RELIABLE (PowerupPickup type = 0x43, FLAG_RELIABLE)
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertPowerupPickup: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

bool PacketManager::assertWeaponFire(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 19 bytes
    if (data.size() != 19) {
        LOG_ERROR_CAT("PacketManager", "assertWeaponFire: payload size != 19, got %zu", data.size());
        return false;
    }

    // Offset 0-3: shooter_id must not be 0
    uint32_t shooter_id;
    std::memcpy(&shooter_id, data.data() + 0, sizeof(uint32_t));
    if (shooter_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertWeaponFire: shooter_id == 0");
        return false;
    }

    // Offset 4-7: projectile_id must not be 0
    uint32_t projectile_id;
    std::memcpy(&projectile_id, data.data() + 4, sizeof(uint32_t));
    if (projectile_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertWeaponFire: projectile_id == 0");
        return false;
    }

    // Offset 8-9, 10-11: origin_x, origin_y - valid coordinates
    // Offset 12-13, 14-15: direction_x, direction_y - normalized vector
    // TODO: Validate normalized direction vector

    // Offset 16: weapon_type must be valid (0x00 to 0x05)
    uint8_t weapon_type;
    std::memcpy(&weapon_type, data.data() + 16, sizeof(uint8_t));
    if (weapon_type > 5) {
        LOG_ERROR_CAT("PacketManager", "assertWeaponFire: invalid weapon_type 0x%02hhx", weapon_type);
        return false;
    }

    // WeaponFire (type = 0x44) does NOT require FLAG_RELIABLE

    return true;
}

bool PacketManager::assertVisualEffect(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 14 bytes (effect_type(1) + pos_x(2) + pos_y(2) + duration_ms(2) + scale(1) + color_tint_r/g/b(3))
    if (data.size() != 14) {
        LOG_ERROR_CAT("PacketManager", "assertVisualEffect: payload size != 14, got %zu", data.size());
        return false;
    }

    // Offset 0: effect_type must be valid (0x00 to 0x0C)
    uint8_t effect_type;
    std::memcpy(&effect_type, data.data() + 0, sizeof(uint8_t));
    if (effect_type > static_cast<uint8_t>(protocol::VisualEffectType::VFX_BOSS_INTRO)) {
        LOG_ERROR_CAT("PacketManager", "assertVisualEffect: invalid effect_type 0x%02hhx", effect_type);
        return false;
    }

    // Offset 1-2: pos_x (int16_t) - valid coordinates
    int16_t pos_x;
    std::memcpy(&pos_x, data.data() + 1, sizeof(int16_t));

    // Offset 3-4: pos_y (int16_t) - valid coordinates
    int16_t pos_y;
    std::memcpy(&pos_y, data.data() + 3, sizeof(int16_t));

    // Offset 5-6: duration_ms must be > 0 and <= 60000
    uint16_t duration_ms;
    std::memcpy(&duration_ms, data.data() + 5, sizeof(uint16_t));
    if (duration_ms == 0 || duration_ms > 60000) {
        LOG_ERROR_CAT("PacketManager", "assertVisualEffect: invalid duration_ms %hu", duration_ms);
        return false;
    }

    // Offset 7: scale must be > 0 and reasonable (1-255)
    uint8_t scale;
    std::memcpy(&scale, data.data() + 7, sizeof(uint8_t));
    if (scale == 0) {
        LOG_ERROR_CAT("PacketManager", "assertVisualEffect: invalid scale %hhu", scale);
        return false;
    }

    // Offset 8: color_tint_r (0-255) - automatically valid if uint8_t
    // Offset 9: color_tint_g (0-255) - automatically valid if uint8_t
    // Offset 10: color_tint_b (0-255) - automatically valid if uint8_t

    // VisualEffect (type = 0x50) does NOT require FLAG_RELIABLE

    return true;
}

bool PacketManager::assertAudioEffect(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 7 bytes
    if (data.size() != 7) {
        LOG_ERROR_CAT("PacketManager", "assertAudioEffect: payload size != 7, got %zu", data.size());
        return false;
    }

    // Offset 0: effect_type must be valid (0x00 to 0x0C)
    uint8_t effect_type;
    std::memcpy(&effect_type, data.data() + 0, sizeof(uint8_t));
    if (effect_type > 0x0C) {
        LOG_ERROR_CAT("PacketManager", "assertAudioEffect: invalid effect_type 0x%02hhx", effect_type);
        return false;
    }

    // Offset 1-2, 3-4: pos_x, pos_y - valid coordinates (for 3D audio)
    // Offset 5: volume (0-255) - automatically valid
    // Offset 6: pitch must be between 50 and 200 (0.5x to 2x speed)
    uint8_t pitch;
    std::memcpy(&pitch, data.data() + 6, sizeof(uint8_t));
    if (pitch < 50 || pitch > 200) {
        LOG_ERROR_CAT("PacketManager", "assertAudioEffect: pitch not in [50,200], got %hhu", pitch);
        return false;
    }

    // AudioEffect (type = 0x51) does NOT require FLAG_RELIABLE

    return true;
}

bool PacketManager::assertParticleSpawn(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 26 bytes (particle_system_id(2) + pos_x(2) + pos_y(2) + velocity_x(2) + velocity_y(2) + particle_count(2) + lifetime_ms(2) + color_start_r/g/b(3) + color_end_r/g/b(3))
    if (data.size() != 26) {
        LOG_ERROR_CAT("PacketManager", "assertParticleSpawn: payload size != 26, got %zu", data.size());
        return false;
    }

    // Offset 0-1: particle_system_id
    uint16_t particle_system_id;
    std::memcpy(&particle_system_id, data.data() + 0, sizeof(uint16_t));

    // Offset 2-3: pos_x (int16_t)
    int16_t pos_x;
    std::memcpy(&pos_x, data.data() + 2, sizeof(int16_t));

    // Offset 4-5: pos_y (int16_t)
    int16_t pos_y;
    std::memcpy(&pos_y, data.data() + 4, sizeof(int16_t));

    // Offset 6-7: velocity_x (int16_t)
    int16_t velocity_x;
    std::memcpy(&velocity_x, data.data() + 6, sizeof(int16_t));

    // Offset 8-9: velocity_y (int16_t)
    int16_t velocity_y;
    std::memcpy(&velocity_y, data.data() + 8, sizeof(int16_t));

    // Offset 10-11: particle_count must be > 0
    uint16_t particle_count;
    std::memcpy(&particle_count, data.data() + 10, sizeof(uint16_t));
    if (particle_count == 0) {
        LOG_ERROR_CAT("PacketManager", "assertParticleSpawn: particle_count == 0");
        return false;
    }

    // Offset 12-13: lifetime_ms must be > 0
    uint16_t lifetime_ms;
    std::memcpy(&lifetime_ms, data.data() + 12, sizeof(uint16_t));
    if (lifetime_ms == 0) {
        LOG_ERROR_CAT("PacketManager", "assertParticleSpawn: lifetime_ms == 0");
        return false;
    }

    // Offset 14: color_start_r (0-255)
    // Offset 15: color_start_g (0-255)
    // Offset 16: color_start_b (0-255)
    // Offset 17: color_end_r (0-255)
    // Offset 18: color_end_g (0-255)
    // Offset 19: color_end_b (0-255)
    // RGB values automatically valid as uint8_t

    // ParticleSpawn (type = 0x52) does NOT require FLAG_RELIABLE

    return true;
}

// ==============================================================
//                  GAME_CONTROL (0x60-0x6F)
// ==============================================================

bool PacketManager::assertGameStart(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 23 bytes (game_instance_id(4) + player_count(1) + player_ids[4](16) + level_id(1) + difficulty(1))
    if (data.size() != 23) {
        LOG_ERROR_CAT("PacketManager", "assertGameStart: payload size != 23, got %zu", data.size());
        return false;
    }

    // Offset 0-3: game_instance_id must not be 0
    uint32_t game_instance_id;
    std::memcpy(&game_instance_id, data.data() + 0, sizeof(uint32_t));
    if (game_instance_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertGameStart: game_instance_id == 0");
        return false;
    }

    // Offset 4: player_count must be 1-4
    uint8_t player_count;
    std::memcpy(&player_count, data.data() + 4, sizeof(uint8_t));
    if (player_count < 1 || player_count > 4) {
        LOG_ERROR_CAT("PacketManager", "assertGameStart: player_count out of range (1-4), got %hhu", player_count);
        return false;
    }

    // Must have FLAG_RELIABLE (GameStart type = 0x60, FLAG_RELIABLE)
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertGameStart: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

bool PacketManager::assertGameEnd(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 22 bytes (end_reason(1) + final_scores[4](16) + winner_id(1) + play_time(4))
    if (data.size() != 22) {
        LOG_ERROR_CAT("PacketManager", "assertGameEnd: payload size != 22, got %zu", data.size());
        return false;
    }

    // Offset 0: end_reason must be valid GameEndReasons
    uint8_t end_reason;
    std::memcpy(&end_reason, data.data() + 0, sizeof(uint8_t));
    if (end_reason > static_cast<uint8_t>(protocol::GameEndReasons::GAME_END_SERVER_SHUTDOWN)) {
        LOG_ERROR_CAT("PacketManager", "assertGameEnd: invalid end_reason 0x%02hhx", end_reason);
        return false;
    }

    // Must have FLAG_RELIABLE (GameEnd type = 0x61, FLAG_RELIABLE)
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertGameEnd: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

bool PacketManager::assertLevelComplete(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 8 bytes (completed_level(1) + next_level(1) + bonus_score(4) + completion_time(2))
    if (data.size() != 8) {
        LOG_ERROR_CAT("PacketManager", "assertLevelComplete: payload size != 8, got %zu", data.size());
        return false;
    }

    // Offset 0: completed_level must be valid
    uint8_t completed_level;
    std::memcpy(&completed_level, data.data() + 0, sizeof(uint8_t));
    // completed_level can be any value

    // Offset 1: next_level (0xFF = game end, otherwise valid level ID)
    uint8_t next_level;
    std::memcpy(&next_level, data.data() + 1, sizeof(uint8_t));

    // Must have FLAG_RELIABLE (LevelComplete type = 0x62, FLAG_RELIABLE)
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertLevelComplete: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

bool PacketManager::assertLevelStart(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 35 bytes (level_id(1) + level_name[32](32) + estimated_duration(2))
    if (data.size() != 35) {
        LOG_ERROR_CAT("PacketManager", "assertLevelStart: payload size != 35, got %zu", data.size());
        return false;
    }

    // Offset 0: level_id must not be 0
    uint8_t level_id;
    std::memcpy(&level_id, data.data() + 0, sizeof(uint8_t));
    if (level_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertLevelStart: level_id == 0");
        return false;
    }

    // Offset 1-32: level_name (char[32]) - must be non-empty and null-terminated
    const char *level_name = reinterpret_cast<const char *>(data.data() + 1);
    bool is_empty = true;
    for (int i = 0; i < 31; ++i) {
        if (level_name[i] != '\0') {
            is_empty = false;
            break;
        }
    }
    if (is_empty) {
        LOG_ERROR_CAT("PacketManager", "assertLevelStart: level_name is empty");
        return false;
    }

    // Offset 33-34: estimated_duration (uint16_t)
    uint16_t estimated_duration;
    std::memcpy(&estimated_duration, data.data() + 33, sizeof(uint16_t));
    // estimated_duration can be any value (0 means unknown)

    // Must have FLAG_RELIABLE (LevelStart type = 0x63, FLAG_RELIABLE)
    if ((header.flags & 0x01) == 0) {
        LOG_ERROR_CAT("PacketManager", "assertLevelStart: missing FLAG_RELIABLE");
        return false;
    }

    return true;
}

bool PacketManager::assertForceState(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 12 bytes (force_entity_id(4) + parent_ship_id(4) + attachment_point(1) + power_level(1) + charge_percentage(1) + is_firing(1))
    if (data.size() != 12) {
        LOG_ERROR_CAT("PacketManager", "assertForceState: payload size != 12, got %zu", data.size());
        return false;
    }

    // Offset 0-3: force_entity_id must not be 0
    uint32_t force_entity_id;
    std::memcpy(&force_entity_id, data.data() + 0, sizeof(uint32_t));
    if (force_entity_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertForceState: force_entity_id == 0");
        return false;
    }

    // Offset 4-7: parent_ship_id (0 = detached, otherwise valid ship ID)
    uint32_t parent_ship_id;
    std::memcpy(&parent_ship_id, data.data() + 4, sizeof(uint32_t));
    // parent_ship_id can be 0 (detached)

    // Offset 8: attachment_point (0=front, 1=back, 2=detached)
    uint8_t attachment_point;
    std::memcpy(&attachment_point, data.data() + 8, sizeof(uint8_t));
    if (attachment_point > 2) {
        LOG_ERROR_CAT("PacketManager", "assertForceState: invalid attachment_point %hhu", attachment_point);
        return false;
    }

    // Offset 9: power_level (1-5)
    uint8_t power_level;
    std::memcpy(&power_level, data.data() + 9, sizeof(uint8_t));
    if (power_level < 1 || power_level > 5) {
        LOG_ERROR_CAT("PacketManager", "assertForceState: invalid power_level %hhu (1-5)", power_level);
        return false;
    }

    // Offset 10: charge_percentage (0-100)
    uint8_t charge_percentage;
    std::memcpy(&charge_percentage, data.data() + 10, sizeof(uint8_t));
    if (charge_percentage > 100) {
        LOG_ERROR_CAT("PacketManager", "assertForceState: charge_percentage > 100, got %hhu", charge_percentage);
        return false;
    }

    // Offset 11: is_firing (boolean)
    uint8_t is_firing;
    std::memcpy(&is_firing, data.data() + 11, sizeof(uint8_t));
    // is_firing is boolean (0 or 1)

    // ForceState (type = 0x64) does NOT require FLAG_RELIABLE

    return true;
}

bool PacketManager::assertAIState(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 18 bytes (entity_id(4) + current_state(1) + behavior_type(1) + target_entity_id(4) + waypoint_x(2) + waypoint_y(2) + state_timer(2))
    if (data.size() != 18) {
        LOG_ERROR_CAT("PacketManager", "assertAIState: payload size != 18, got %zu", data.size());
        return false;
    }

    // Offset 0-3: entity_id must not be 0
    uint32_t entity_id;
    std::memcpy(&entity_id, data.data() + 0, sizeof(uint32_t));
    if (entity_id == 0) {
        LOG_ERROR_CAT("PacketManager", "assertAIState: entity_id == 0");
        return false;
    }

    // Offset 4: current_state (0=idle, 1=patrol, 2=chase, 3=attack, 4=retreat, 5=dead)
    uint8_t current_state;
    std::memcpy(&current_state, data.data() + 4, sizeof(uint8_t));
    if (current_state > 5) {
        LOG_ERROR_CAT("PacketManager", "assertAIState: invalid current_state %hhu", current_state);
        return false;
    }

    // Offset 5: behavior_type (0=melee, 1=ranged, 2=support, 3=boss)
    uint8_t behavior_type;
    std::memcpy(&behavior_type, data.data() + 5, sizeof(uint8_t));
    if (behavior_type > 3) {
        LOG_ERROR_CAT("PacketManager", "assertAIState: invalid behavior_type %hhu", behavior_type);
        return false;
    }

    // Offset 6-9: target_entity_id (0 = no target)
    uint32_t target_entity_id;
    std::memcpy(&target_entity_id, data.data() + 6, sizeof(uint32_t));
    // target_entity_id can be 0

    // Offset 10-11: waypoint_x (int16)
    int16_t waypoint_x;
    std::memcpy(&waypoint_x, data.data() + 10, sizeof(int16_t));
    // No range validation on waypoint

    // Offset 12-13: waypoint_y (int16)
    int16_t waypoint_y;
    std::memcpy(&waypoint_y, data.data() + 12, sizeof(int16_t));
    // No range validation on waypoint

    // Offset 14-15: state_timer (uint16, milliseconds)
    uint16_t state_timer;
    std::memcpy(&state_timer, data.data() + 14, sizeof(uint16_t));
    // state_timer can be any value (0 = expired)

    // AIState (type = 0x65) does NOT require FLAG_RELIABLE

    return true;
}

// ==============================================================
//                  PROTOCOL_CONTROL (0x70-0x7F)
// ==============================================================

bool PacketManager::assertAcknowledgment(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 8 bytes (acked_sequence + received_timestamp)
    if (data.size() != 8) {
        LOG_ERROR_CAT("PacketManager", "assertAcknowledgment: payload size != 8, got %zu", data.size());
        return false;
    }

    // Offset 0-3: acked_sequence (uint32_t)
    uint32_t acked_sequence;
    std::memcpy(&acked_sequence, data.data() + 0, sizeof(uint32_t));
    // No strict validation needed - can be any sequence number

    // Offset 4-7: received_timestamp (uint32_t)
    uint32_t received_timestamp;
    std::memcpy(&received_timestamp, data.data() + 4, sizeof(uint32_t));
    // No strict validation needed - just timestamp

    return true;
}

bool PacketManager::assertPing(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 4 bytes (client_timestamp)
    if (data.size() != 4) {
        LOG_ERROR_CAT("PacketManager", "assertPing: payload size != 4, got %zu", data.size());
        return false;
    }

    // Offset 0-3: client_timestamp (uint32_t)
    uint32_t client_timestamp;
    std::memcpy(&client_timestamp, data.data() + 0, sizeof(uint32_t));
    // No strict validation needed - just check it exists

    return true;
}

bool PacketManager::assertPong(const common::protocol::Packet &packet)
{
    const auto &data = packet.data;
    const auto &header = packet.header;

    // Payload: 8 bytes (client_timestamp + server_timestamp)
    if (data.size() != 8) {
        LOG_ERROR_CAT("PacketManager", "assertPong: payload size != 8, got %zu", data.size());
        return false;
    }

    // Offset 0-3: client_timestamp (uint32_t) - echo from PING
    uint32_t client_timestamp;
    std::memcpy(&client_timestamp, data.data() + 0, sizeof(uint32_t));

    // Offset 4-7: server_timestamp (uint32_t)
    uint32_t server_timestamp;
    std::memcpy(&server_timestamp, data.data() + 4, sizeof(uint32_t));
    // No strict validation needed - just check it exists

    return true;
}

// ==============================================================
//                  Creation Functions (Serialization)
// ==============================================================

// ==============================================================
//                  CONNECTION (0x01-0x0F)
// ==============================================================

common::protocol::Packet PacketManager::createClientConnect(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_CONNECT));
    return packet;
}

common::protocol::Packet PacketManager::createServerAccept(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_ACCEPT));
    return packet;
}

common::protocol::Packet PacketManager::createServerReject(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_REJECT));
    return packet;
}

common::protocol::Packet PacketManager::createClientDisconnect(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_DISCONNECT));
    return packet;
}

common::protocol::Packet PacketManager::createHeartBeat(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEARTBEAT));
    return packet;
}

// ==============================================================
//                  INPUT (0x10-0x1F)
// ==============================================================

common::protocol::Packet PacketManager::createPlayerInput(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_INPUT));
    return packet;
}

// ==============================================================
//                  WORLD_STATE (0x20-0x3F)
// ==============================================================

/* common::protocol::Packet NetworkManager::createWorldSnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_WORLD_SNAPSHOT));
    return packet;
} */

common::protocol::Packet PacketManager::createEntitySpawn(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_SPAWN));
    return packet;
}

common::protocol::Packet PacketManager::createEntityDestroy(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_DESTROY));
    return packet;
}

/* common::protocol::Packet NetworkManager::createEntityUpdate(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_UPDATE));
    return packet;
} */

common::protocol::Packet PacketManager::createTransformSnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT));
    return packet;
}

common::protocol::Packet PacketManager::createVelocitySnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_VELOCITY_SNAPSHOT));
    return packet;
}

common::protocol::Packet PacketManager::createHealthSnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT));
    return packet;
}

common::protocol::Packet PacketManager::createWeaponSnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_SNAPSHOT));
    return packet;
}

common::protocol::Packet PacketManager::createAISnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_AI_SNAPSHOT));
    return packet;
}

common::protocol::Packet PacketManager::createAnimationSnapshot(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ANIMATION_SNAPSHOT));
    return packet;
}

common::protocol::Packet PacketManager::createComponentAdd(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_COMPONENT_ADD));
    return packet;
}

common::protocol::Packet PacketManager::createComponentRemove(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_COMPONENT_REMOVE));
    return packet;
}

common::protocol::Packet PacketManager::createTransformSnapshotDelta(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT_DELTA));
    return packet;
}

common::protocol::Packet PacketManager::createHealthSnapshotDelta(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT_DELTA));
    return packet;
}

common::protocol::Packet PacketManager::createEntityFullState(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_FULL_STATE));
    return packet;
}

// ==============================================================
//                  GAME_EVENTS (0x40-0x5F)
// ==============================================================

common::protocol::Packet PacketManager::createPlayerHit(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_HIT));
    return packet;
}

common::protocol::Packet PacketManager::createPlayerDeath(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_DEATH));
    return packet;
}

common::protocol::Packet PacketManager::createScoreUpdate(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_SCORE_UPDATE));
    return packet;
}

common::protocol::Packet PacketManager::createPowerupPickup(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_POWER_PICKUP));
    return packet;
}

common::protocol::Packet PacketManager::createWeaponFire(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_FIRE));
    return packet;
}

common::protocol::Packet PacketManager::createVisualEffect(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_VISUAL_EFFECT));
    return packet;
}

common::protocol::Packet PacketManager::createAudioEffect(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_AUDIO_EFFECT));
    return packet;
}

common::protocol::Packet PacketManager::createParticleSpawn(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PARTICLE_SPAWN));
    return packet;
}

// ==============================================================
//                  GAME_CONTROL (0x60-0x6F)
// ==============================================================

common::protocol::Packet PacketManager::createGameStart(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_GAME_START));
    return packet;
}

common::protocol::Packet PacketManager::createGameEnd(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_GAME_END));
    return packet;
}

common::protocol::Packet PacketManager::createLevelComplete(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_LEVEL_COMPLETE));
    return packet;
}

common::protocol::Packet PacketManager::createLevelStart(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_LEVEL_START));
    return packet;
}

common::protocol::Packet PacketManager::createForceState(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_FORCE_STATE));
    return packet;
}

common::protocol::Packet PacketManager::createAIState(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_AI_STATE));
    return packet;
}

// ==============================================================
//                  PROTOCOL_CONTROL (0x70-0x7F)
// ==============================================================

common::protocol::Packet PacketManager::createAcknowledgment(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ACK));
    return packet;
}

common::protocol::Packet PacketManager::createPing(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PING));
    return packet;
}

common::protocol::Packet PacketManager::createPong(const std::vector<uint8_t> &args)
{
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PONG));
    return packet;
}

