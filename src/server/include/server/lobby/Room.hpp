#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace server {
namespace lobby {

enum class RoomState : uint8_t {
    WAITING = 0,
    STARTING = 1,
    IN_PROGRESS = 2,
    FINISHED = 3
};

class Room {
public:
    Room(uint32_t id, const std::string& name, uint8_t maxPlayers);

    uint32_t getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    uint8_t getMaxPlayers() const { return m_maxPlayers; }
    uint8_t getCurrentPlayers() const { return m_playerIds.size(); }
    RoomState getState() const { return m_state; }

    bool addPlayer(uint32_t playerId);
    bool removePlayer(uint32_t playerId);
    bool isFull() const { return getCurrentPlayers() >= m_maxPlayers; }

    void startGame();
    void endGame();

private:
    uint32_t m_id;
    std::string m_name;
    uint8_t m_maxPlayers;
    std::vector<uint32_t> m_playerIds;
    RoomState m_state;
};

} // namespace lobby
} // namespace server
