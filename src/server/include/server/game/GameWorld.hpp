#pragma once

#include <memory>
#include <cstdint>

namespace server {
namespace game {

class GameWorld {
public:
    GameWorld();
    ~GameWorld();

    void initialize();
    void update(float deltaTime);
    void shutdown();

    // Entity management
    uint32_t createEntity();
    void destroyEntity(uint32_t entityId);

    // World state
    void getWorldState(std::vector<uint8_t>& buffer);

private:
    uint32_t m_nextEntityId = 1;
    uint32_t m_currentTick = 0;
};

} // namespace game
} // namespace server
