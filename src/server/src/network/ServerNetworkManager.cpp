/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ServerNetworkManager
*/

#include "../../include/server/network/ServerNetworkManager.hpp"
#include "common/protocol/Protocol.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

namespace server {
namespace network {

ServerNetworkManager::ServerNetworkManager(uint16_t basePort, uint32_t maxPlayers)
    : _basePort(basePort),
      _maxPlayers(std::min<uint32_t>(maxPlayers, MAX_PLAYERS)),
      _clients(_maxPlayers),
      _running(false)
{
    for (uint32_t i = 0; i < _maxPlayers; ++i) {
        _clients[i].socket = std::make_shared<common::network::AsioSocket>();
    }
}

ServerNetworkManager::~ServerNetworkManager()
{
    stop();
}

void ServerNetworkManager::start()
{
    if (_running.exchange(true)) {
        return;
    }

    for (uint32_t i = 0; i < _maxPlayers; ++i) {
        auto& slot = _clients[i];
        slot.socket->setNonBlocking(true);
        if (!slot.socket->bind(static_cast<uint16_t>(_basePort + i))) {
            std::cerr << "[ServerNetworkManager] Failed to bind port " << (_basePort + i) << std::endl;
        }
    }

    _thread = std::thread(&ServerNetworkManager::networkLoop, this);
}

void ServerNetworkManager::stop()
{
    if (!_running.exchange(false)) {
        return;
    }
    if (_thread.joinable()) {
        _thread.join();
    }
    for (auto& slot : _clients) {
        if (slot.socket) {
            slot.socket->close();
        }
        slot.active = false;
    }
}

void ServerNetworkManager::queueOutgoing(const common::protocol::Packet& packet,
                                         std::optional<uint32_t> targetClient)
{
    std::lock_guard<std::mutex> lock(_outMutex);
    _outgoing.emplace_back(packet, targetClient);
}

std::vector<common::network::ReceivedPacket> ServerNetworkManager::fetchIncoming()
{
    std::lock_guard<std::mutex> lock(_inMutex);
    std::vector<common::network::ReceivedPacket> packets(_incoming.begin(), _incoming.end());
    _incoming.clear();
    return packets;
}

bool ServerNetworkManager::shouldForward(const common::protocol::Packet& packet) const
{
    const auto type = static_cast<protocol::PacketTypes>(packet.header.packet_type);
    switch (type) {
        case protocol::PacketTypes::TYPE_CLIENT_CONNECT:
        case protocol::PacketTypes::TYPE_SERVER_ACCEPT:
        case protocol::PacketTypes::TYPE_SERVER_REJECT:
        case protocol::PacketTypes::TYPE_CLIENT_DISCONNECT:
        case protocol::PacketTypes::TYPE_HEARTBEAT:
        case protocol::PacketTypes::TYPE_ACK:
        case protocol::PacketTypes::TYPE_PING:
        case protocol::PacketTypes::TYPE_PONG:
            return false;
        default:
            return true;
    }
}

void ServerNetworkManager::ensureConnected(ClientSlot& slot)
{
    if (slot.active) {
        return;
    }
    if (!slot.socket->getRemoteAddress().empty()) {
        slot.socket->connect(slot.socket->getRemoteAddress(), slot.socket->getRemotePort());
        slot.active = true;
    }
}

void ServerNetworkManager::networkLoop()
{
    while (_running.load()) {
        for (uint32_t i = 0; i < _maxPlayers; ++i) {
            auto& slot = _clients[i];
            common::protocol::Packet incoming;
            if (slot.socket->receive(incoming)) {
                ensureConnected(slot);
                if (shouldForward(incoming)) {
                    std::lock_guard<std::mutex> lock(_inMutex);
                    _incoming.push_back({incoming, i});
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(_outMutex);
            while (!_outgoing.empty()) {
                const auto [packet, target] = _outgoing.front();
                if (target.has_value()) {
                    uint32_t idx = target.value();
                    if (idx < _clients.size() && _clients[idx].active) {
                        _clients[idx].socket->send(packet);
                    }
                } else {
                    for (auto& slot : _clients) {
                        if (slot.active) {
                            slot.socket->send(packet);
                        }
                    }
                }
                _outgoing.pop_front();
            }
        }

        std::this_thread::sleep_for(1ms);
    }
}

} // namespace network
} // namespace server
