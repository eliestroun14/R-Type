#pragma once

#include <common/protocol/Packet.hpp>
#include <vector>

namespace common {
namespace protocol {
namespace packets {

struct EntityState {
    uint32_t entityId;
    float x, y;
    float velocityX, velocityY;
    float rotation;
    uint8_t state;
};

struct WorldState {
    uint32_t tick;
    std::vector<EntityState> entities;
};

struct EntitySpawn {
    uint32_t entityId;
    uint8_t entityType;
    float x, y;
};

struct EntityDestroy {
    uint32_t entityId;
};

struct EntityUpdate {
    EntityState state;
};

} // namespace packets
} // namespace protocol
} // namespace common
