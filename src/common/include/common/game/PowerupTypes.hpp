#pragma once

#include <cstdint>

namespace common {
namespace game {

enum class PowerupType : uint8_t {
    HEALTH = 0,
    SHIELD = 1,
    WEAPON_UPGRADE = 2,
    SPEED_BOOST = 3,
    INVINCIBILITY = 4,
    EXTRA_LIFE = 5,
    SCORE_MULTIPLIER = 6
};

struct PowerupEffect {
    PowerupType type;
    float duration;
    float value;
};

} // namespace game
} // namespace common
