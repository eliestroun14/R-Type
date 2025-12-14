#pragma once

#include <cstdint>

namespace common {
namespace protocol {

enum class PacketType : uint8_t {
    // Connection packets (0x01-0x0F)
    CONNECT_REQUEST = 0x01,
    CONNECT_RESPONSE = 0x02,
    DISCONNECT = 0x03,
    HEARTBEAT = 0x04,
    HEARTBEAT_ACK = 0x05,

    // Input packets (0x10-0x1F)
    PLAYER_INPUT = 0x10,
    INPUT_ACK = 0x11,

    // World state packets (0x20-0x3F)
    WORLD_STATE = 0x20,
    ENTITY_SPAWN = 0x21,
    ENTITY_DESTROY = 0x22,
    ENTITY_UPDATE = 0x23,

    // Game event packets (0x40-0x5F)
    PLAYER_SHOOT = 0x40,
    PLAYER_HIT = 0x41,
    PLAYER_DEATH = 0x42,
    ENEMY_DEATH = 0x43,
    POWERUP_COLLECTED = 0x44,

    // Game control packets (0x60-0x6F)
    GAME_START = 0x60,
    GAME_PAUSE = 0x61,
    GAME_RESUME = 0x62,
    GAME_END = 0x63,

    // Protocol packets (0x70-0x7F)
    ACK = 0x70,
    NACK = 0x71,
    PING = 0x72,
    PONG = 0x73
};

} // namespace protocol
} // namespace common
