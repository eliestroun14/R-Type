#pragma once

#include <cstdint>

namespace common {
namespace game {

// Game configuration constants
constexpr uint16_t DEFAULT_PORT = 8080;
constexpr uint16_t MAX_PLAYERS = 4;
constexpr uint32_t TICK_RATE = 60;
constexpr float FIXED_TIMESTEP = 1.0f / TICK_RATE;

// Network constants
constexpr uint32_t MAX_PACKET_SIZE = 1400;
constexpr uint32_t HEARTBEAT_INTERVAL_MS = 1000;
constexpr uint32_t CONNECTION_TIMEOUT_MS = 5000;

// Game world constants
constexpr float WORLD_WIDTH = 1920.0f;
constexpr float WORLD_HEIGHT = 1080.0f;

// Player constants
constexpr float PLAYER_SPEED = 300.0f;
constexpr uint16_t PLAYER_MAX_HEALTH = 100;
constexpr float PLAYER_FIRE_RATE = 0.5f;

// Enemy constants
constexpr float ENEMY_SPAWN_RATE = 2.0f;
constexpr uint16_t ENEMY_MAX_HEALTH = 50;

} // namespace game
} // namespace common
