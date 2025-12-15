/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ServerNetworkManager
*/

#ifndef SERVERNETWORKMANAGER_HPP_
#define SERVERNETWORKMANAGER_HPP_

#include <atomic>
#include <deque>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>
#include <map>
#include <string>
#include <common/constants/defines.hpp>
#include <common/network/NetworkManager.hpp>
#include <common/network/sockets/AsioSocket.hpp>

// Forward declaration
namespace gameEngine {
    class GameEngine;
}

namespace server {
namespace network {

class ServerNetworkManager : public common::network::INetworkManager {
public:
    ServerNetworkManager(uint16_t basePort, uint32_t maxPlayers);
    ~ServerNetworkManager() override;

    void start() override;
    void stop() override;
    bool isRunning() const override { return _running.load(); }

    void queueOutgoing(const common::protocol::Packet& packet,
                       std::optional<uint32_t> targetClient = std::nullopt) override;

    std::vector<common::network::ReceivedPacket> fetchIncoming() override;

    void run();

    // Set the game engine reference for entity creation
    void setGameEngine(std::shared_ptr<gameEngine::GameEngine> engine) { _gameEngine = engine; }

private:
    struct ClientSlot {
        std::string remoteAddress;  // "ip:port" of the client
        uint32_t clientId;
        bool active = false;
        uint64_t lastHeartbeatTime = 0;
    };

    bool shouldForward(const common::protocol::Packet& packet) const;
    void checkClientTimeouts();
    void handleNetworkPacket(const common::protocol::Packet& packet, const std::string& remoteAddress, uint64_t currentMs);
    
    // Individual handlers for each packet type
    void handleClientConnect(const std::string& remoteAddress, uint64_t currentMs);
    void handleClientDisconnect(const std::string& remoteAddress);
    void handleHeartbeat(const std::string& remoteAddress, uint64_t currentMs);
    void handlePing(const common::protocol::Packet& packet, const std::string& remoteAddress);
    
    // Find or assign a client ID based on remote address
    std::optional<uint32_t> findClientIdByAddress(const std::string& remoteAddress);
    std::optional<uint32_t> findFreeSlot();

    /**
     * @brief Create a player entity on the server only
     * @param playerId The player ID
     * @param posX Starting X position
     * @param posY Starting Y position
     */
    void createPlayerEntity(uint32_t playerId, uint16_t posX, uint16_t posY);

    /**
     * @brief Create a bullet entity on the server only
     * @param bulletId The bullet ID
     * @param posX Starting X position
     * @param posY Starting Y position
     * @param velX Velocity X
     * @param velY Velocity Y
     */
    void createBulletEntity(uint32_t bulletId, uint16_t posX, uint16_t posY,
                            int16_t velX, int16_t velY);

    /**
     * @brief Send a player to a specific client with is_playable=true
     * @param playerId The player ID
     * @param targetClientId The client to send to
     * @param posX Player X position
     * @param posY Player Y position
     */
    void sendPlayerToClient(uint32_t playerId, uint32_t targetClientId,
                            uint16_t posX, uint16_t posY);

    /**
     * @brief Send a player to all other clients with is_playable=false
     * @param playerId The player ID
     * @param originatingClientId The client that should be excluded
     * @param posX Player X position
     * @param posY Player Y position
     */
    void sendPlayerToOtherClients(uint32_t playerId, uint32_t originatingClientId,
                                  uint16_t posX, uint16_t posY);

    /**
     * @brief Send a bullet to all clients except the originating one
     * @param bulletId The bullet ID
     * @param originatingClientId The client that should be excluded
     * @param posX Bullet X position
     * @param posY Bullet Y position
     * @param velX Bullet velocity X
     * @param velY Bullet velocity Y
     */
    void sendBulletToOtherClients(uint32_t bulletId, uint32_t originatingClientId,
                                  uint16_t posX, uint16_t posY,
                                  int16_t velX, int16_t velY);

private:
    /**
     * @brief Internal: Create an entity on the server only
     * @param entityType The type of entity to create (from protocol::EntityTypes)
     * @param entityId The ID for this entity
     * @param posX Starting X position
     * @param posY Starting Y position
     * @param mobVariant Entity variant (0 for default)
     * @param initialHealth Starting health points
     * @param initialVelX Starting velocity X
     * @param initialVelY Starting velocity Y
     */
    void spawnEntity(uint8_t entityType, uint32_t entityId,
                     uint16_t posX, uint16_t posY, uint8_t mobVariant,
                     uint8_t initialHealth, int16_t initialVelX, int16_t initialVelY);

    uint16_t _basePort;
    uint32_t _maxPlayers;
    std::vector<ClientSlot> _clients;
    std::map<std::string, uint32_t> _addressToClientId;  // Maps "ip:port" to clientId

    std::shared_ptr<common::network::AsioSocket> _acceptorSocket;  // Single socket listening on basePort
    std::atomic<bool> _running;

    std::shared_ptr<gameEngine::GameEngine> _gameEngine;  // Reference to game engine for entity creation

    std::mutex _inMutex;
    std::mutex _outMutex;
    std::deque<common::network::ReceivedPacket> _incoming;
    std::deque<std::pair<common::protocol::Packet, std::optional<uint32_t>>> _outgoing;
};

} // namespace network
} // namespace server

#endif /* !SERVERNETWORKMANAGER_HPP_ */

