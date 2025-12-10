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
#include <thread>
#include "../../../common/include/common/network/NetworkManager.hpp"
#include "../../../common/include/common/network/sockets/AsioSocket.hpp"

namespace client {
namespace network {

class ClientNetworkManager : public common::network::INetworkManager {
public:
    ClientNetworkManager(const std::string& host, uint16_t port);
    ~ClientNetworkManager() override;

    void start() override;
    void stop() override;
    bool isRunning() const override { return _running.load(); }

    void queueOutgoing(const common::protocol::Packet& packet,
                       std::optional<uint32_t> targetClient = std::nullopt) override;

    std::vector<common::network::ReceivedPacket> fetchIncoming() override;

private:
    void networkLoop();
    bool shouldForward(const common::protocol::Packet& packet) const;

    std::string _host;
    uint16_t _port;

    std::shared_ptr<common::network::AsioSocket> _socket;
    std::atomic<bool> _running;
    std::thread _thread;

    std::mutex _inMutex;
    std::mutex _outMutex;
    std::deque<common::network::ReceivedPacket> _incoming;
    std::deque<common::protocol::Packet> _outgoing;
};

} // namespace network
} // namespace client

#endif /* !CLIENTNETWORKMANAGER_HPP_ */
