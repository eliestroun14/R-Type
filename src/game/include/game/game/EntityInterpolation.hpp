#pragma once

#include <unordered_map>
#include <cstdint>

namespace client {
namespace game {

struct EntitySnapshot {
    float x, y;
    float velocityX, velocityY;
    float rotation;
    uint32_t timestamp;
};

class EntityInterpolation {
public:
    void addSnapshot(uint32_t entityId, const EntitySnapshot& snapshot);
    EntitySnapshot interpolate(uint32_t entityId, uint32_t timestamp);

    void setInterpolationDelay(uint32_t delayMs) { m_interpolationDelay = delayMs; }

private:
    std::unordered_map<uint32_t, std::vector<EntitySnapshot>> m_snapshots;
    uint32_t m_interpolationDelay = 100;
};

} // namespace game
} // namespace client
