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

    uint16_t _basePort;
    uint32_t _maxPlayers;
    std::vector<ClientSlot> _clients;
    std::map<std::string, uint32_t> _addressToClientId;  // Maps "ip:port" to clientId

    std::shared_ptr<common::network::AsioSocket> _acceptorSocket;  // Single socket listening on basePort
    std::atomic<bool> _running;

    std::mutex _inMutex;
    std::mutex _outMutex;
    std::deque<common::network::ReceivedPacket> _incoming;
    std::deque<std::pair<common::protocol::Packet, std::optional<uint32_t>>> _outgoing;
};

} // namespace network
} // namespace server

#endif /* !SERVERNETWORKMANAGER_HPP_ */

