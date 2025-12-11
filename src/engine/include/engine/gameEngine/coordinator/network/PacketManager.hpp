/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PacketManager
*/

#ifndef PACKETMANAGER_HPP_
#define PACKETMANAGER_HPP_

#include <cstdint>
#include <vector>
#include <map>
#include <optional>
#include <array>
#include <common/protocol/Protocol.hpp>
#include <common/protocol/Packet.hpp>

/**
 * @class PacketManager
 * @brief Manages network packet processing and creation for the ECS
 *
 * The PacketManager is responsible for:
 * - Processing (deserializing and validating) incoming network packets
 * - Creating (building and serializing) outgoing network packets
 *
 * It uses a compile-time optimized handler table (constexpr std::array)
 * to dispatch packet types to their respective handlers with minimal overhead.
 */
class PacketManager {
    public:
        // ==============================================================
        //                          Initialization
        // ==============================================================

        PacketManager() = default;
        ~PacketManager() = default;

        // ==============================================================
        //                        Packet Processing
        // ==============================================================

        /**
         * @brief Process a received network packet (deserialize and validate)
         * @param packet The packet to process
         * @return The deserialized and validated packet, or std::nullopt if validation failed
         */
        std::optional<common::protocol::Packet> processPacket(const common::protocol::Packet &packet);

        /**
         * @brief Create a network packet to send
         * @param type The packet type to create
         * @param args Additional arguments for packet creation
         * @return The created packet, or std::nullopt if validation failed
         */
        std::optional<common::protocol::Packet> createPacket(protocol::PacketTypes type, const std::vector<uint8_t> &args = {});

        // ==============================================================
        //                  Assertion Functions (Validation)
        // ==============================================================
        // These are public static methods to allow testing of packet validation logic

        // ==============================================================
        //                  CONNECTION (0x01-0x0F)
        // ==============================================================

        /**
         * @brief Validate a CLIENT_CONNECT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertClientConnect(const common::protocol::Packet &packet);

        /**
         * @brief Validate a SERVER_ACCEPT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertServerAccept(const common::protocol::Packet &packet);

        /**
         * @brief Validate a SERVER_REJECT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertServerReject(const common::protocol::Packet &packet);

        /**
         * @brief Validate a CLIENT_DISCONNECT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertClientDisconnect(const common::protocol::Packet &packet);

        /**
         * @brief Validate a HEARTBEAT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertHeartBeat(const common::protocol::Packet &packet);

        // ==============================================================
        //                  INPUT (0x10-0x1F)
        // ==============================================================

        /**
         * @brief Validate a PLAYER_INPUT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertPlayerInput(const common::protocol::Packet &packet);

        // ==============================================================
        //                  WORLD_STATE (0x20-0x3F)
        // ==============================================================

        /**
         * @brief Validate a WORLD_SNAPSHOT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertWorldSnapshot(const common::protocol::Packet &packet);

        /**
         * @brief Validate an ENTITY_SPAWN packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertEntitySpawn(const common::protocol::Packet &packet);

        /**
         * @brief Validate an ENTITY_DESTROY packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertEntityDestroy(const common::protocol::Packet &packet);

        /**
         * @brief Validate an ENTITY_UPDATE packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertEntityUpdate(const common::protocol::Packet &packet);

        /**
         * @brief Validate a TRANSFORM_SNAPSHOT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertTransformSnapshot(const common::protocol::Packet &packet);

        /**
         * @brief Validate a VELOCITY_SNAPSHOT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertVelocitySnapshot(const common::protocol::Packet &packet);

        /**
         * @brief Validate a HEALTH_SNAPSHOT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertHealthSnapshot(const common::protocol::Packet &packet);

        /**
         * @brief Validate a WEAPON_SNAPSHOT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertWeaponSnapshot(const common::protocol::Packet &packet);

        /**
         * @brief Validate an AI_SNAPSHOT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertAISnapshot(const common::protocol::Packet &packet);

        /**
         * @brief Validate an ANIMATION_SNAPSHOT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertAnimationSnapshot(const common::protocol::Packet &packet);

        /**
         * @brief Validate a COMPONENT_ADD packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertComponentAdd(const common::protocol::Packet &packet);

        /**
         * @brief Validate a COMPONENT_REMOVE packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertComponentRemove(const common::protocol::Packet &packet);

        /**
         * @brief Validate a TRANSFORM_SNAPSHOT_DELTA packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertTransformSnapshotDelta(const common::protocol::Packet &packet);

        /**
         * @brief Validate a HEALTH_SNAPSHOT_DELTA packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertHealthSnapshotDelta(const common::protocol::Packet &packet);

        /**
         * @brief Validate an ENTITY_FULL_STATE packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertEntityFullState(const common::protocol::Packet &packet);

        // ==============================================================
        //                  GAME_EVENTS (0x40-0x5F)
        // ==============================================================

        /**
         * @brief Validate a PLAYER_HIT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertPlayerHit(const common::protocol::Packet &packet);

        /**
         * @brief Validate a PLAYER_DEATH packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertPlayerDeath(const common::protocol::Packet &packet);

        /**
         * @brief Validate a SCORE_UPDATE packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertScoreUpdate(const common::protocol::Packet &packet);

        /**
         * @brief Validate a POWERUP_PICKUP packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertPowerupPickup(const common::protocol::Packet &packet);

        /**
         * @brief Validate a WEAPON_FIRE packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertWeaponFire(const common::protocol::Packet &packet);

        /**
         * @brief Validate a VISUAL_EFFECT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertVisualEffect(const common::protocol::Packet &packet);

        /**
         * @brief Validate an AUDIO_EFFECT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertAudioEffect(const common::protocol::Packet &packet);

        /**
         * @brief Validate a PARTICLE_SPAWN packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertParticleSpawn(const common::protocol::Packet &packet);

        // ==============================================================
        //                  GAME_CONTROL (0x60-0x6F)
        // ==============================================================

        /**
         * @brief Validate a GAME_START packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertGameStart(const common::protocol::Packet &packet);

        /**
         * @brief Validate a GAME_END packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertGameEnd(const common::protocol::Packet &packet);

        /**
         * @brief Validate a LEVEL_COMPLETE packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertLevelComplete(const common::protocol::Packet &packet);

        /**
         * @brief Validate a LEVEL_START packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertLevelStart(const common::protocol::Packet &packet);

        /**
         * @brief Validate a FORCE_STATE packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertForceState(const common::protocol::Packet &packet);

        /**
         * @brief Validate an AI_STATE packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertAIState(const common::protocol::Packet &packet);

        // ==============================================================
        //                  PROTOCOL_CONTROL (0x70-0x7F)
        // ==============================================================

        /**
         * @brief Validate an ACKNOWLEDGMENT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertAcknowledgment(const common::protocol::Packet &packet);

        /**
         * @brief Validate a PING packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertPing(const common::protocol::Packet &packet);

        /**
         * @brief Validate a PONG packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertPong(const common::protocol::Packet &packet);

        // ==============================================================
        //                  Creation Functions
        // ==============================================================

        // ==============================================================
        //                  CONNECTION (0x01-0x0F)
        // ==============================================================

        /**
         * @brief Create a CLIENT_CONNECT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t) - number of flags to combine
         *   - [1..flags_count]: flag values (uint8_t each) - combined with bitwise OR
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+40]: player_name (32 bytes, char array)
         *   - [flags_count+41..flags_count+44]: client_id (uint32_t, little-endian)
         *
         * Total minimum size: 46 bytes (when flags_count = 0)
         *
         * @return The created CLIENT_CONNECT packet with 37 bytes payload
         */
        static std::optional<common::protocol::Packet> createClientConnect(const std::vector<uint8_t> &args);

        /**
         * @brief Create a SERVER_ACCEPT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t) - number of flags to combine
         *   - [1..flags_count]: flag values (uint8_t each) - combined with bitwise OR
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: assigned_player_id (uint32_t, little-endian)
         *   - [flags_count+13]: max_players (uint8_t)
         *   - [flags_count+14..flags_count+17]: game_instance_id (uint32_t, little-endian)
         *   - [flags_count+18..flags_count+19]: server_tickrate (uint16_t, little-endian)
         *
         * Total minimum size: 18 bytes (when flags_count = 0)
         *
         * @return The created SERVER_ACCEPT packet with 11 bytes payload
         */
        static std::optional<common::protocol::Packet> createServerAccept(const std::vector<uint8_t> &args);

        /**
         * @brief Create a SERVER_REJECT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9]: reject_code (uint8_t)
         *   - [flags_count+10..flags_count+73]: reason_message (64 bytes, char array)
         *
         * Total minimum size: 72 bytes (when flags_count = 0)
         *
         * @return The created SERVER_REJECT packet with 65 bytes payload
         */
        static std::optional<common::protocol::Packet> createServerReject(const std::vector<uint8_t> &args);

        /**
         * @brief Create a CLIENT_DISCONNECT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: player_id (uint32_t, little-endian)
         *   - [flags_count+13]: reason (uint8_t, DisconnectReasons enum)
         *
         * Total minimum size: 12 bytes (when flags_count = 0)
         *
         * @return The created CLIENT_DISCONNECT packet with 5 bytes payload
         */
        static std::optional<common::protocol::Packet> createClientDisconnect(const std::vector<uint8_t> &args);

        /**
         * @brief Create a HEARTBEAT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: player_id (uint32_t, little-endian)
         *
         * Total minimum size: 11 bytes (when flags_count = 0)
         *
         * @return The created HEARTBEAT packet with 4 bytes payload
         */
        static std::optional<common::protocol::Packet> createHeartBeat(const std::vector<uint8_t> &args);

        // ==============================================================
        //                  INPUT (0x10-0x1F)
        // ==============================================================

        /**
         * @brief Create a PLAYER_INPUT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: player_id (uint32_t, little-endian)
         *   - [flags_count+13..flags_count+14]: input_state (uint16_t, little-endian, InputFlags bitfield)
         *   - [flags_count+15..flags_count+16]: aim_direction_x (uint16_t, little-endian)
         *   - [flags_count+17..flags_count+18]: aim_direction_y (uint16_t, little-endian)
         *
         * Total minimum size: 19 bytes (when flags_count = 0)
         *
         * @return The created PLAYER_INPUT packet with 12 bytes payload
         */
        static std::optional<common::protocol::Packet> createPlayerInput(const std::vector<uint8_t> &args);

        // ==============================================================
        //                  WORLD_STATE (0x20-0x3F)
        // ==============================================================

        /**
         * @brief Create an ENTITY_SPAWN packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: entity_id (uint32_t, little-endian)
         *   - [flags_count+13]: entity_type (uint8_t, EntityTypes enum)
         *   - [flags_count+14..flags_count+15]: position_x (uint16_t, little-endian)
         *   - [flags_count+16..flags_count+17]: position_y (uint16_t, little-endian)
         *   - [flags_count+18]: mob_variant (uint8_t)
         *   - [flags_count+19]: initial_health (uint8_t)
         *   - [flags_count+20..flags_count+21]: initial_velocity_x (uint16_t, little-endian)
         *   - [flags_count+22..flags_count+23]: initial_velocity_y (uint16_t, little-endian)
         *
         * Total minimum size: 24 bytes (when flags_count = 0)
         *
         * @return The created ENTITY_SPAWN packet with 15 bytes payload
         */
        static std::optional<common::protocol::Packet> createEntitySpawn(const std::vector<uint8_t> &args);

        /**
         * @brief Create an ENTITY_DESTROY packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: entity_id (uint32_t, little-endian)
         *   - [flags_count+13]: destroy_reason (uint8_t, EntityDestroyReasons enum)
         *   - [flags_count+14..flags_count+15]: final_position_x (uint16_t, little-endian)
         *   - [flags_count+16..flags_count+17]: final_position_y (uint16_t, little-endian)
         *
         * Total minimum size: 16 bytes (when flags_count = 0)
         *
         * @return The created ENTITY_DESTROY packet with 9 bytes payload
         */
        static std::optional<common::protocol::Packet> createEntityDestroy(const std::vector<uint8_t> &args);

        /**
         * @brief Create a TRANSFORM_SNAPSHOT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..]: world_tick, entity_count, and variable entity data
         *
         * @return The created TRANSFORM_SNAPSHOT packet with variable payload
         */
        static std::optional<common::protocol::Packet> createTransformSnapshot(const std::vector<uint8_t> &args);

        /**
         * @brief Create a VELOCITY_SNAPSHOT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..]: world_tick, entity_count, and variable entity data
         *
         * @return The created VELOCITY_SNAPSHOT packet with variable payload
         */
        static std::optional<common::protocol::Packet> createVelocitySnapshot(const std::vector<uint8_t> &args);

        /**
         * @brief Create a HEALTH_SNAPSHOT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..]: world_tick, entity_count, and variable entity data
         *
         * @return The created HEALTH_SNAPSHOT packet with variable payload
         */
        static std::optional<common::protocol::Packet> createHealthSnapshot(const std::vector<uint8_t> &args);

        /**
         * @brief Create a WEAPON_SNAPSHOT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..]: world_tick, entity_count, and variable entity data
         *
         * @return The created WEAPON_SNAPSHOT packet with variable payload
         */
        static std::optional<common::protocol::Packet> createWeaponSnapshot(const std::vector<uint8_t> &args);

        /**
         * @brief Create an AI_SNAPSHOT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..]: world_tick, entity_count, and variable entity data
         *
         * @return The created AI_SNAPSHOT packet with variable payload
         */
        static std::optional<common::protocol::Packet> createAISnapshot(const std::vector<uint8_t> &args);

        /**
         * @brief Create an ANIMATION_SNAPSHOT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..]: world_tick, entity_count, and variable entity data
         *
         * @return The created ANIMATION_SNAPSHOT packet with variable payload
         */
        static std::optional<common::protocol::Packet> createAnimationSnapshot(const std::vector<uint8_t> &args);

        /**
         * @brief Create a COMPONENT_ADD packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..]: entity_id, component_type, data_size, and variable component data
         *
         * @return The created COMPONENT_ADD packet with variable payload
         */
        static std::optional<common::protocol::Packet> createComponentAdd(const std::vector<uint8_t> &args);

        /**
         * @brief Create a COMPONENT_REMOVE packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: entity_id (uint32_t, little-endian)
         *   - [flags_count+13]: component_type (uint8_t, ComponentType enum)
         *
         * Total minimum size: 12 bytes (when flags_count = 0)
         *
         * @return The created COMPONENT_REMOVE packet with 5 bytes payload
         */
        static std::optional<common::protocol::Packet> createComponentRemove(const std::vector<uint8_t> &args);

        /**
         * @brief Create a TRANSFORM_SNAPSHOT_DELTA packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..]: world_tick, base_tick, entity_count, and variable entity data
         *
         * @return The created TRANSFORM_SNAPSHOT_DELTA packet with variable payload
         */
        static std::optional<common::protocol::Packet> createTransformSnapshotDelta(const std::vector<uint8_t> &args);

        /**
         * @brief Create a HEALTH_SNAPSHOT_DELTA packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..]: world_tick, base_tick, entity_count, and variable entity data
         *
         * @return The created HEALTH_SNAPSHOT_DELTA packet with variable payload
         */
        static std::optional<common::protocol::Packet> createHealthSnapshotDelta(const std::vector<uint8_t> &args);

        /**
         * @brief Create an ENTITY_FULL_STATE packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..]: entity_id, entity_type, component_count, and variable component data
         *
         * @return The created ENTITY_FULL_STATE packet with variable payload
         */
        static std::optional<common::protocol::Packet> createEntityFullState(const std::vector<uint8_t> &args);

        // ==============================================================
        //                  GAME_EVENTS (0x40-0x5F)
        // ==============================================================

        /**
         * @brief Create a PLAYER_HIT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: victim_id (uint32_t, little-endian)
         *   - [flags_count+13..flags_count+16]: attacker_id (uint32_t, little-endian)
         *   - [flags_count+17]: damage (uint8_t)
         *
         * Total minimum size: 17 bytes (when flags_count = 0)
         *
         * @return The created PLAYER_HIT packet with 9 bytes payload
         */
        static std::optional<common::protocol::Packet> createPlayerHit(const std::vector<uint8_t> &args);

        /**
         * @brief Create a PLAYER_DEATH packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: victim_id (uint32_t, little-endian)
         *   - [flags_count+13..flags_count+16]: killer_id (uint32_t, little-endian)
         *
         * Total minimum size: 16 bytes (when flags_count = 0)
         *
         * @return The created PLAYER_DEATH packet with 8 bytes payload
         */
        static std::optional<common::protocol::Packet> createPlayerDeath(const std::vector<uint8_t> &args);

        /**
         * @brief Create a SCORE_UPDATE packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: player_id (uint32_t, little-endian)
         *   - [flags_count+13..flags_count+16]: score (uint32_t, little-endian)
         *
         * Total minimum size: 16 bytes (when flags_count = 0)
         *
         * @return The created SCORE_UPDATE packet with 8 bytes payload
         */
        static std::optional<common::protocol::Packet> createScoreUpdate(const std::vector<uint8_t> &args);

        /**
         * @brief Create a POWERUP_PICKUP packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: player_id (uint32_t, little-endian)
         *   - [flags_count+13..flags_count+16]: powerup_id (uint32_t, little-endian)
         *   - [flags_count+17]: powerup_type (uint8_t, PowerupType enum)
         *
         * Total minimum size: 17 bytes (when flags_count = 0)
         *
         * @return The created POWERUP_PICKUP packet with 9 bytes payload
         */
        static std::optional<common::protocol::Packet> createPowerupPickup(const std::vector<uint8_t> &args);

        /**
         * @brief Create a WEAPON_FIRE packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: shooter_id (uint32_t, little-endian)
         *   - [flags_count+13..flags_count+16]: weapon_id (uint32_t, little-endian)
         *   - [flags_count+17..flags_count+20]: target_x (float, IEEE 754)
         *   - [flags_count+21..flags_count+24]: target_y (float, IEEE 754)
         *
         * Total minimum size: 24 bytes (when flags_count = 0)
         *
         * @return The created WEAPON_FIRE packet with 16 bytes payload
         */
        static std::optional<common::protocol::Packet> createWeaponFire(const std::vector<uint8_t> &args);

        /**
         * @brief Create a VISUAL_EFFECT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9]: effect_type (uint8_t, EffectType enum)
         *   - [flags_count+10..flags_count+13]: position_x (float, IEEE 754)
         *   - [flags_count+14..flags_count+17]: position_y (float, IEEE 754)
         *   - [flags_count+18]: intensity (uint8_t)
         *
         * Total minimum size: 18 bytes (when flags_count = 0)
         *
         * @return The created VISUAL_EFFECT packet with 10 bytes payload
         */
        static std::optional<common::protocol::Packet> createVisualEffect(const std::vector<uint8_t> &args);

        /**
         * @brief Create an AUDIO_EFFECT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9]: sound_type (uint8_t, SoundType enum)
         *   - [flags_count+10..flags_count+13]: position_x (float, IEEE 754)
         *   - [flags_count+14..flags_count+17]: position_y (float, IEEE 754)
         *   - [flags_count+18]: volume (uint8_t)
         *
         * Total minimum size: 18 bytes (when flags_count = 0)
         *
         * @return The created AUDIO_EFFECT packet with 10 bytes payload
         */
        static std::optional<common::protocol::Packet> createAudioEffect(const std::vector<uint8_t> &args);

        /**
         * @brief Create a PARTICLE_SPAWN packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9]: particle_type (uint8_t, ParticleType enum)
         *   - [flags_count+10..flags_count+13]: position_x (float, IEEE 754)
         *   - [flags_count+14..flags_count+17]: position_y (float, IEEE 754)
         *   - [flags_count+18..flags_count+21]: velocity_x (float, IEEE 754)
         *   - [flags_count+22..flags_count+25]: velocity_y (float, IEEE 754)
         *   - [flags_count+26]: lifetime_ms (uint8_t)
         *
         * Total minimum size: 26 bytes (when flags_count = 0)
         *
         * @return The created PARTICLE_SPAWN packet with 18 bytes payload
         */
        static std::optional<common::protocol::Packet> createParticleSpawn(const std::vector<uint8_t> &args);

        // ==============================================================
        //                  GAME_CONTROL (0x60-0x6F)
        // ==============================================================

        /**
         * @brief Create a GAME_START packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: game_id (uint32_t, little-endian)
         *   - [flags_count+13..flags_count+16]: world_seed (uint32_t, little-endian)
         *   - [flags_count+17..flags_count+20]: max_players (uint32_t, little-endian)
         *   - [flags_count+21..flags_count+24]: level_index (uint32_t, little-endian)
         *   - [flags_count+25]: difficulty (uint8_t, Difficulty enum)
         *
         * Total minimum size: 25 bytes (when flags_count = 0)
         *
         * @return The created GAME_START packet with 17 bytes payload
         */
        static std::optional<common::protocol::Packet> createGameStart(const std::vector<uint8_t> &args);

        /**
         * @brief Create a GAME_END packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: game_id (uint32_t, little-endian)
         *   - [flags_count+13]: end_reason (uint8_t, EndReason enum)
         *   - [flags_count+14..flags_count+17]: winner_id (uint32_t, little-endian, or 0 for draw)
         *   - [flags_count+18..flags_count+21]: total_duration_seconds (uint32_t, little-endian)
         *
         * Total minimum size: 21 bytes (when flags_count = 0)
         *
         * @return The created GAME_END packet with 13 bytes payload
         */
        static std::optional<common::protocol::Packet> createGameEnd(const std::vector<uint8_t> &args);

        /**
         * @brief Create a LEVEL_COMPLETE packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: game_id (uint32_t, little-endian)
         *   - [flags_count+13..flags_count+16]: level_index (uint32_t, little-endian)
         *   - [flags_count+17..flags_count+20]: completion_time_seconds (uint32_t, little-endian)
         *
         * Total minimum size: 20 bytes (when flags_count = 0)
         *
         * @return The created LEVEL_COMPLETE packet with 12 bytes payload
         */
        static std::optional<common::protocol::Packet> createLevelComplete(const std::vector<uint8_t> &args);

        /**
         * @brief Create a LEVEL_START packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: game_id (uint32_t, little-endian)
         *   - [flags_count+13..flags_count+16]: level_index (uint32_t, little-endian)
         *   - [flags_count+17..flags_count+20]: initial_difficulty_modifier (float, IEEE 754)
         *   - [flags_count+21..flags_count+24]: enemy_count (uint32_t, little-endian)
         *   - [flags_count+25]: environment_type (uint8_t, EnvironmentType enum)
         *
         * Total minimum size: 25 bytes (when flags_count = 0)
         *
         * @return The created LEVEL_START packet with 17 bytes payload
         */
        static std::optional<common::protocol::Packet> createLevelStart(const std::vector<uint8_t> &args);

        /**
         * @brief Create a FORCE_STATE packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: entity_id (uint32_t, little-endian)
         *   - [flags_count+13..flags_count+16]: force_x (float, IEEE 754)
         *   - [flags_count+17..flags_count+20]: force_y (float, IEEE 754)
         *   - [flags_count+21]: force_type (uint8_t, ForceType enum)
         *
         * Total minimum size: 21 bytes (when flags_count = 0)
         *
         * @return The created FORCE_STATE packet with 13 bytes payload
         */
        static std::optional<common::protocol::Packet> createForceState(const std::vector<uint8_t> &args);

        /**
         * @brief Create an AI_STATE packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: entity_id (uint32_t, little-endian)
         *   - [flags_count+13]: ai_state (uint8_t, AIState enum)
         *   - [flags_count+14..flags_count+17]: target_id (uint32_t, little-endian, or 0 for none)
         *   - [flags_count+18..flags_count+21]: behavior_parameter (float, IEEE 754)
         *
         * Total minimum size: 21 bytes (when flags_count = 0)
         *
         * @return The created AI_STATE packet with 13 bytes payload
         */
        static std::optional<common::protocol::Packet> createAIState(const std::vector<uint8_t> &args);

        // ==============================================================
        //                  PROTOCOL_CONTROL (0x70-0x7F)
        // ==============================================================

        /**
         * @brief Create an ACKNOWLEDGMENT packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: acknowledged_sequence (uint32_t, little-endian)
         *
         * Total minimum size: 12 bytes (when flags_count = 0)
         *
         * @return The created ACKNOWLEDGMENT packet with 4 bytes payload
         */
        static std::optional<common::protocol::Packet> createAcknowledgment(const std::vector<uint8_t> &args);

        /**
         * @brief Create a PING packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *
         * Total minimum size: 8 bytes (when flags_count = 0)
         *
         * @return The created PING packet with 0 bytes payload
         */
        static std::optional<common::protocol::Packet> createPing(const std::vector<uint8_t> &args);

        /**
         * @brief Create a PONG packet
         *
         * @param args Packed arguments vector with the following structure:
         *   - [0]: flags_count (uint8_t)
         *   - [1..flags_count]: flag values (uint8_t each)
         *   - [flags_count+1..flags_count+4]: sequence_number (uint32_t, little-endian)
         *   - [flags_count+5..flags_count+8]: timestamp (uint32_t, little-endian)
         *   - [flags_count+9..flags_count+12]: ping_sequence (uint32_t, little-endian)
         *
         * Total minimum size: 12 bytes (when flags_count = 0)
         *
         * @return The created PONG packet with 4 bytes payload
         */
        static std::optional<common::protocol::Packet> createPong(const std::vector<uint8_t> &args);

    private:

        // ==============================================================
        //                  Packet Handler Structure
        // ==============================================================

        /**
         * @struct PacketHandler
         * @brief Associates a packet type with its validation and creation functions
         *
         * Used by the handlers table to map packet types to their respective
         * assertion (validation) and creation (serialization) functions.
         */
        struct PacketHandler {
            protocol::PacketTypes type;
            bool (*assertFunc)(const common::protocol::Packet &);
            std::optional<common::protocol::Packet> (*createFunc)(const std::vector<uint8_t> &);
        };

        /**
         * @brief Static handler table for all supported packet types
         *
         * This table maps each supported packet type to its assertion and creation functions.
         * It is initialized at compile-time (constexpr) for optimal performance and memory usage.
         * Each entry contains:
         * - The packet type identifier
         * - A pointer to the assertion function (validates packet data)
         * - A pointer to the creation function (builds/serializes packets)
         *
         * @details Exemple handlers:
         * - TYPE_ENTITY_SPAWN: Handles entity spawn events
         * - TYPE_WORLD_SNAPSHOT: Handles world state synchronization
         *
         * To add new packet types, simply add a new entry to this array with
         * the corresponding assertion and creation functions.
         */
        static constexpr std::array<PacketHandler, 38> handlers = {{
            // CONNECTION (0x01-0x0F)
            { protocol::PacketTypes::TYPE_CLIENT_CONNECT, &PacketManager::assertClientConnect, &PacketManager::createClientConnect },
            { protocol::PacketTypes::TYPE_SERVER_ACCEPT, &PacketManager::assertServerAccept, &PacketManager::createServerAccept },
            { protocol::PacketTypes::TYPE_SERVER_REJECT, &PacketManager::assertServerReject, &PacketManager::createServerReject },
            { protocol::PacketTypes::TYPE_CLIENT_DISCONNECT, &PacketManager::assertClientDisconnect, &PacketManager::createClientDisconnect },
            { protocol::PacketTypes::TYPE_HEARTBEAT, &PacketManager::assertHeartBeat, &PacketManager::createHeartBeat },

            // INPUT (0x10-0x1F)
            { protocol::PacketTypes::TYPE_PLAYER_INPUT, &PacketManager::assertPlayerInput, &PacketManager::createPlayerInput },

            // WORLD_STATE (0x20-0x3F)
            { protocol::PacketTypes::TYPE_ENTITY_SPAWN, &PacketManager::assertEntitySpawn, &PacketManager::createEntitySpawn },
            { protocol::PacketTypes::TYPE_ENTITY_DESTROY, &PacketManager::assertEntityDestroy, &PacketManager::createEntityDestroy },
            { protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT, &PacketManager::assertTransformSnapshot, &PacketManager::createTransformSnapshot },
            { protocol::PacketTypes::TYPE_VELOCITY_SNAPSHOT, &PacketManager::assertVelocitySnapshot, &PacketManager::createVelocitySnapshot },
            { protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT, &PacketManager::assertHealthSnapshot, &PacketManager::createHealthSnapshot },
            { protocol::PacketTypes::TYPE_WEAPON_SNAPSHOT, &PacketManager::assertWeaponSnapshot, &PacketManager::createWeaponSnapshot },
            { protocol::PacketTypes::TYPE_AI_SNAPSHOT, &PacketManager::assertAISnapshot, &PacketManager::createAISnapshot },
            { protocol::PacketTypes::TYPE_ANIMATION_SNAPSHOT, &PacketManager::assertAnimationSnapshot, &PacketManager::createAnimationSnapshot },
            { protocol::PacketTypes::TYPE_COMPONENT_ADD, &PacketManager::assertComponentAdd, &PacketManager::createComponentAdd },
            { protocol::PacketTypes::TYPE_COMPONENT_REMOVE, &PacketManager::assertComponentRemove, &PacketManager::createComponentRemove },
            { protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT_DELTA, &PacketManager::assertTransformSnapshotDelta, &PacketManager::createTransformSnapshotDelta },
            { protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT_DELTA, &PacketManager::assertHealthSnapshotDelta, &PacketManager::createHealthSnapshotDelta },
            { protocol::PacketTypes::TYPE_ENTITY_FULL_STATE, &PacketManager::assertEntityFullState, &PacketManager::createEntityFullState },

            // GAME_EVENTS (0x40-0x5F)
            { protocol::PacketTypes::TYPE_PLAYER_HIT, &PacketManager::assertPlayerHit, &PacketManager::createPlayerHit },
            { protocol::PacketTypes::TYPE_PLAYER_DEATH, &PacketManager::assertPlayerDeath, &PacketManager::createPlayerDeath },
            { protocol::PacketTypes::TYPE_SCORE_UPDATE, &PacketManager::assertScoreUpdate, &PacketManager::createScoreUpdate },
            { protocol::PacketTypes::TYPE_POWER_PICKUP, &PacketManager::assertPowerupPickup, &PacketManager::createPowerupPickup },
            { protocol::PacketTypes::TYPE_WEAPON_FIRE, &PacketManager::assertWeaponFire, &PacketManager::createWeaponFire },
            { protocol::PacketTypes::TYPE_VISUAL_EFFECT, &PacketManager::assertVisualEffect, &PacketManager::createVisualEffect },
            { protocol::PacketTypes::TYPE_AUDIO_EFFECT, &PacketManager::assertAudioEffect, &PacketManager::createAudioEffect },
            { protocol::PacketTypes::TYPE_PARTICLE_SPAWN, &PacketManager::assertParticleSpawn, &PacketManager::createParticleSpawn },

            // GAME_CONTROL (0x60-0x6F)
            { protocol::PacketTypes::TYPE_GAME_START, &PacketManager::assertGameStart, &PacketManager::createGameStart },
            { protocol::PacketTypes::TYPE_GAME_END, &PacketManager::assertGameEnd, &PacketManager::createGameEnd },
            { protocol::PacketTypes::TYPE_LEVEL_COMPLETE, &PacketManager::assertLevelComplete, &PacketManager::createLevelComplete },
            { protocol::PacketTypes::TYPE_LEVEL_START, &PacketManager::assertLevelStart, &PacketManager::createLevelStart },
            { protocol::PacketTypes::TYPE_FORCE_STATE, &PacketManager::assertForceState, &PacketManager::createForceState },
            { protocol::PacketTypes::TYPE_AI_STATE, &PacketManager::assertAIState, &PacketManager::createAIState },

            // PROTOCOL_CONTROL (0x70-0x7F)
            { protocol::PacketTypes::TYPE_ACK, &PacketManager::assertAcknowledgment, &PacketManager::createAcknowledgment },
            { protocol::PacketTypes::TYPE_PING, &PacketManager::assertPing, &PacketManager::createPing },
            { protocol::PacketTypes::TYPE_PONG, &PacketManager::assertPong, &PacketManager::createPong }
        }};

    public:
        /**
         * @brief Find a packet handler by type
         * @param type The packet type to find
         * @return Pointer to the handler, or nullptr if not found
         */
        static PacketHandler *findHandler(protocol::PacketTypes type);
};

#endif /* !PACKETMANAGER_HPP_ */
