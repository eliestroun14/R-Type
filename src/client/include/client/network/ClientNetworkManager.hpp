/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ClientNetworkManager
*/

#ifndef CLIENTNETWORKMANAGER_HPP_
#define CLIENTNETWORKMANAGER_HPP_

#include <atomic>
#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <common/network/NetworkManager.hpp>
#include <common/network/sockets/AsioSocket.hpp>

class RTypeClient;  // Forward declaration

namespace client {
namespace network {

class ClientNetworkManager : public common::network::INetworkManager {
public:
    ClientNetworkManager(const std::string& host, uint16_t port, RTypeClient* client = nullptr);
    ~ClientNetworkManager() override;

    void start() override;
    void stop() override;
    bool isRunning() const override { return _running.load(); }

    void queueOutgoing(const common::protocol::Packet& packet,
                       std::optional<uint32_t> targetClient = std::nullopt) override;

    std::vector<common::network::ReceivedPacket> fetchIncoming() override;

    void run();

    // Network packet sending methods
    void sendConnectionRequest();
    void sendHeartbeat();
    void sendDisconnect(uint8_t reason);
    void sendAck(uint32_t acked_sequence, uint32_t received_timestamp);
    void sendPing();

    // handling methods
    void handleConnectionAccepted(const common::protocol::Packet& packet);
    void handleConnectionRejected(const common::protocol::Packet& packet);
    //void handleHeartbeatAck();
    void handleDisconnect(const common::protocol::Packet& packet);
    void handleAck(const common::protocol::Packet& packet);
    void handlePing(const common::protocol::Packet& packet);
    void handlePong(const common::protocol::Packet& packet);



private:
    bool shouldForward(const common::protocol::Packet& packet) const;
    void handleNetworkPacket(const common::protocol::Packet& packet);

    std::string _host;
    uint16_t _port;
    RTypeClient* _client;

    std::shared_ptr<common::network::AsioSocket> _socket;
    std::atomic<bool> _running;
    std::atomic<bool> _connected;

    std::mutex _inMutex;
    std::mutex _outMutex;
    std::deque<common::network::ReceivedPacket> _incoming;
    std::deque<common::protocol::Packet> _outgoing;

    std::atomic<unsigned int> _tickCount;
};

} // namespace network
} // namespace client

#endif /* !CLIENTNETWORKMANAGER_HPP_ */
