#pragma once

#include "Room.hpp"
#include <memory>

namespace server {
namespace lobby {

class Matchmaking {
public:
    std::shared_ptr<Room> findMatch(uint32_t playerId, uint8_t preferredPlayers = 4);
    void addToQueue(uint32_t playerId);
    void removeFromQueue(uint32_t playerId);

private:
    std::vector<uint32_t> m_queue;
};

} // namespace lobby
} // namespace server
