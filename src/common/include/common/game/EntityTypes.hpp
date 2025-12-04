#pragma once

#include <cstdint>

namespace common {
namespace game {

enum class EntityType : uint8_t {
    UNKNOWN = 0,
    PLAYER = 1,
    ENEMY_BASIC = 10,
    ENEMY_FAST = 11,
    ENEMY_TANK = 12,
    ENEMY_BOSS = 20,
    PROJECTILE_PLAYER = 30,
    PROJECTILE_ENEMY = 31,
    POWERUP = 40,
    OBSTACLE = 50
};

} // namespace game
} // namespace common
