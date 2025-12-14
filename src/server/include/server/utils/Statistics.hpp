#pragma once

#include <cstdint>

namespace server {
namespace utils {

struct Statistics {
    // Server stats
    uint32_t uptime = 0;
    uint32_t totalConnections = 0;
    uint32_t activeConnections = 0;

    // Game stats
    uint32_t totalGamesPlayed = 0;
    uint32_t activeGames = 0;

    // Network stats
    uint64_t bytesSent = 0;
    uint64_t bytesReceived = 0;
    uint32_t packetsSent = 0;
    uint32_t packetsReceived = 0;

    // Performance stats
    float averageTickTime = 0.0f;
    float averageFPS = 0.0f;

    void reset();
    void print() const;
};

} // namespace utils
} // namespace server
