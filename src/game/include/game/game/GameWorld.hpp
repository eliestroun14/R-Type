#pragma once

#include <memory>
#include <cstdint>

namespace client {
namespace game {

class GameWorld {
public:
    GameWorld();
    ~GameWorld();

    void initialize();
    void update(float deltaTime);
    void render();
    void shutdown();

    // Entity management
    uint32_t createEntity();
    void destroyEntity(uint32_t entityId);

    // World state synchronization
    void applyServerState(const std::vector<uint8_t>& stateData);

private:
    uint32_t m_localPlayerId = 0;
};

} // namespace game
} // namespace client
