#pragma once

#include <common/protocol/Packet.hpp>

namespace common {
namespace protocol {
namespace packets {

struct PlayerShoot {
    uint32_t playerId;
    float directionX;
    float directionY;
    uint8_t weaponType;
};

struct PlayerHit {
    uint32_t playerId;
    uint32_t attackerId;
    uint16_t damage;
    uint16_t remainingHealth;
};

struct PlayerDeath {
    uint32_t playerId;
    uint32_t killerId;
};

struct EnemyDeath {
    uint32_t enemyId;
    uint32_t killerId;
    uint16_t score;
};

struct PowerupCollected {
    uint32_t playerId;
    uint32_t powerupId;
    uint8_t powerupType;
};

} // namespace packets
} // namespace protocol
} // namespace common
