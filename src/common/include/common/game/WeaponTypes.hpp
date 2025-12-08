#pragma once

#include <cstdint>

namespace common {
namespace game {

enum class WeaponType : uint8_t {
    BASIC = 0,
    RAPID_FIRE = 1,
    SPREAD_SHOT = 2,
    LASER = 3,
    MISSILE = 4,
    CHARGE_SHOT = 5
};

struct WeaponStats {
    float damage;
    float fireRate;
    float projectileSpeed;
    uint8_t projectileCount;
};

} // namespace game
} // namespace common
