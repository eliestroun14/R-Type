/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Payload - Network packet payload structures
*/

#ifndef PAYLOAD_HPP_
#define PAYLOAD_HPP_

#include <cstdint>

namespace protocol {

// ============================================================================
// PAYLOAD STRUCTURES (without PacketHeader)
// ============================================================================
#pragma pack(push, 1)

/**
 * @brief Server accept connection payload
 * Used when server accepts a client connection
 */
struct ServerAcceptPayload {
    uint32_t assigned_player_id;    // Unique player identifier assigned by server
    uint8_t  max_players;           // Maximum players allowed on server
    uint32_t game_instance_id;      // Unique game instance identifier
    uint16_t server_tickrate;       // Server tickrate in Hz
};
// Size: 11 bytes

/**
 * @brief Server reject connection payload
 * Used when server rejects a client connection
 */
struct ServerRejectPayload {
    uint8_t reject_code;            // Rejection reason code
    char    reason_message[64];     // Human-readable rejection message
};
// Size: 65 bytes

/**
 * @brief Client connect payload
 * Used for initial connection and heartbeat
 */
struct ClientConnectPayload {
    uint8_t  protocol_version;      // Protocol version
    char     player_name[32];       // Player name
    uint32_t client_id;             // Client identifier (0 for initial connection)
};
// Size: 37 bytes

/**
 * @brief Client disconnect payload
 * Used when client or server disconnects
 */
struct ClientDisconnectPayload {
    uint32_t client_id;             // Client identifier
    uint8_t  reason;                // Disconnect reason code
};
// Size: 5 bytes

/**
 * @brief Heartbeat payload
 * Used to keep connection alive
 */
struct HeartbeatPayload {
    uint32_t player_id;             // Player identifier
};
// Size: 4 bytes

/**
 * @brief Player input payload
 * Used to send player input to server
 */
struct PlayerInputPayload {
    uint32_t player_id;             // Player identifier
    uint16_t input_state;           // Input state flags
    int16_t  aim_direction_x;       // Aim direction X
    int16_t  aim_direction_y;       // Aim direction Y
    int16_t  client_pos_x;          // Client's current position X (for shoot sync)
    int16_t  client_pos_y;          // Client's current position Y (for shoot sync)
};
// Size: 16 bytes

/**
 * @brief Entity spawn payload
 * Used when server spawns an entity
 */
struct EntitySpawnPayload {
    uint32_t entity_id;             // Entity identifier
    uint8_t  entity_type;           // Entity type
    uint16_t position_x;            // Initial position X
    uint16_t position_y;            // Initial position Y
    uint8_t  mob_variant;           // Mob variant/subtype
    uint8_t  initial_health;        // Initial health
    int16_t  initial_velocity_x;    // Initial velocity X
    int16_t  initial_velocity_y;    // Initial velocity Y
    uint8_t  is_playable;           // Is this entity playable by the client
};
// Size: 16 bytes

/**
 * @brief Entity destroy payload
 * Used when server destroys an entity
 */
struct EntityDestroyPayload {
    uint32_t entity_id;             // Entity identifier
    uint8_t  destroy_reason;        // Destruction reason
    uint16_t final_position_x;      // Final position X
    uint16_t final_position_y;      // Final position Y
};
// Size: 9 bytes

/**
 * @brief Acknowledgment payload
 * Used to acknowledge receipt of reliable packets
 */
struct AcknowledgmentPayload {
    uint32_t acked_sequence;        // Sequence number being acknowledged
    uint32_t received_timestamp;    // Timestamp when packet was received
    uint32_t client_id;             // Client identifier
};
// Size: 12 bytes

/**
 * @brief Ping payload
 * Used to measure latency
 */
struct PingPayload {
    uint32_t client_timestamp;      // Client timestamp
    uint32_t client_id;             // Client identifier
};
// Size: 8 bytes

/**
 * @brief Pong payload
 * Used to respond to ping
 */
struct PongPayload {
    uint32_t client_timestamp;      // Original client timestamp
    uint32_t server_timestamp;      // Server timestamp
};
// Size: 8 bytes

#pragma pack(pop)

} // namespace protocol

#endif // PAYLOAD_HPP_
