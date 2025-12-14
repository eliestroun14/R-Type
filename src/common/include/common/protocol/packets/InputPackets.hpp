#pragma once

#include <common/protocol/Packet.hpp>

namespace common {
namespace protocol {
namespace packets {

enum class InputAction : uint8_t {
    MOVE_UP = 0x01,
    MOVE_DOWN = 0x02,
    MOVE_LEFT = 0x04,
    MOVE_RIGHT = 0x08,
    SHOOT = 0x10,
    USE_ABILITY = 0x20
};

struct PlayerInput {
    uint32_t playerId;
    uint8_t actions;  // Bitfield of InputAction
    float mouseX;
    float mouseY;
    uint32_t timestamp;
};

struct InputAck {
    uint32_t sequence;
    uint32_t timestamp;
};

} // namespace packets
} // namespace protocol
} // namespace common
