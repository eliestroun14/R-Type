#pragma once

#include <string>
#include <vector>
#include <memory>

namespace server {
namespace lobby {

class Room;

class LobbyManager {
public:
    LobbyManager();
    ~LobbyManager();

    std::shared_ptr<Room> createRoom(const std::string& name, uint8_t maxPlayers);
    std::shared_ptr<Room> findRoom(uint32_t roomId);
    bool joinRoom(uint32_t roomId, uint32_t playerId);
    void leaveRoom(uint32_t roomId, uint32_t playerId);

    std::vector<std::shared_ptr<Room>> getAvailableRooms();

private:
    std::vector<std::shared_ptr<Room>> m_rooms;
    uint32_t m_nextRoomId = 1;
};

} // namespace lobby
} // namespace server
