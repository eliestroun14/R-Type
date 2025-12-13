/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ServerNetworkManager
*/

#include <server/network/ServerNetworkManager.hpp>
#include <common/protocol/Protocol.hpp>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <engine/gameEngine/coordinator/network/PacketManager.hpp>

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
}

void ServerNetworkManager::stop()
{
    if (!_running.exchange(false)) {
        return;
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

void ServerNetworkManager::run()
{
    auto startEpoch = std::chrono::steady_clock::now();

    while (_running.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        uint64_t currentMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime.time_since_epoch()).count();

        // Check for client timeouts
        checkClientTimeouts();

        for (uint32_t i = 0; i < _maxPlayers; ++i) {
            auto& slot = _clients[i];
            common::protocol::Packet incoming;
            if (slot.socket->receive(incoming)) {
                if (shouldForward(incoming)) {
                    std::lock_guard<std::mutex> lock(_inMutex);
                    _incoming.push_back({incoming, i});
                } else {
                    // Handle network-level packets (connection, heartbeat, etc.)
                    handleNetworkPacket(incoming, i, currentMs);
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

void ServerNetworkManager::checkClientTimeouts()
{
    auto startEpoch = std::chrono::steady_clock::now();
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    for (uint32_t i = 0; i < _maxPlayers; ++i) {
        auto& slot = _clients[i];
        if (slot.active && slot.lastHeartbeatTime > 0) {
            // Calculate elapsed time since last heartbeat
            // If client hasn't sent heartbeat for TIMEOUT_MS, disconnect it
            uint64_t timeSinceLastHeartbeat = currentTime - slot.lastHeartbeatTime;
            if (timeSinceLastHeartbeat > TIMEOUT_MS) {
                std::cout << "[ServerNetworkManager] Client " << i << " timeout (no heartbeat for " 
                          << timeSinceLastHeartbeat << "ms)" << std::endl;
                slot.active = false;
                slot.socket->close();
            }
        }
    }
}

void ServerNetworkManager::handleNetworkPacket(const common::protocol::Packet& packet, uint32_t clientId, uint64_t currentMs)
{
    const auto type = static_cast<protocol::PacketTypes>(packet.header.packet_type);
    
    switch (type) {
        case protocol::PacketTypes::TYPE_CLIENT_CONNECT: {
            std::cout << "[ServerNetworkManager] Client " << clientId << " connection request received" << std::endl;
            handleClientConnect(clientId, currentMs);
            break;
        }
        
        case protocol::PacketTypes::TYPE_CLIENT_DISCONNECT: {
            std::cout << "[ServerNetworkManager] Client " << clientId << " disconnected" << std::endl;
            handleClientDisconnect(clientId);
            break;
        }
        
        case protocol::PacketTypes::TYPE_HEARTBEAT: {
            std::cout << "[ServerNetworkManager] Heartbeat received from client " << clientId << std::endl;
            handleHeartbeat(clientId, currentMs);
            break;
        }
        
        case protocol::PacketTypes::TYPE_PING: {
            std::cout << "[ServerNetworkManager] Ping received from client " << clientId << std::endl;
            handlePing(packet, clientId);
            break;
        }
        
        case protocol::PacketTypes::TYPE_ACK:
        case protocol::PacketTypes::TYPE_PONG:
            // Acknowledgment received, no action needed
            break;
        
        case protocol::PacketTypes::TYPE_SERVER_ACCEPT:
        case protocol::PacketTypes::TYPE_SERVER_REJECT:
            // These should only be sent by server, not received
            std::cerr << "[ServerNetworkManager] Unexpected packet type from client " << clientId << std::endl;
            break;
        
        default:
            std::cerr << "[ServerNetworkManager] Unknown network packet type: " 
                      << static_cast<int>(packet.header.packet_type) << std::endl;
            break;
    }
}

void ServerNetworkManager::handleClientConnect(uint32_t clientId, uint64_t currentMs)
{
    std::cout << "[ServerNetworkManager] Client " << clientId << " connected" << std::endl;
    _clients[clientId].lastHeartbeatTime = currentMs;
    _clients[clientId].active = true;
    
    // Send server accept to client using PacketManager
    std::vector<uint8_t> args;
    args.push_back(0);  // flags_count
    // sequence_number (4 bytes, little-endian)
    args.push_back(0); args.push_back(0); args.push_back(0); args.push_back(0);
    // timestamp (4 bytes, little-endian)
    args.push_back(0); args.push_back(0); args.push_back(0); args.push_back(0);
    // assigned_player_id (4 bytes, little-endian)
    uint32_t id = clientId;
    args.push_back(id & 0xFF);
    args.push_back((id >> 8) & 0xFF);
    args.push_back((id >> 16) & 0xFF);
    args.push_back((id >> 24) & 0xFF);
    // max_players (1 byte)
    args.push_back(_maxPlayers);
    // game_instance_id (4 bytes, little-endian) - use 1 for now
    args.push_back(1); args.push_back(0); args.push_back(0); args.push_back(0);
    // server_tickrate (2 bytes, little-endian)
    uint16_t tickrate = 60;
    args.push_back(tickrate & 0xFF);
    args.push_back((tickrate >> 8) & 0xFF);
    
    auto packet = PacketManager::createServerAccept(args);
    if (packet) {
        queueOutgoing(packet.value(), clientId);
        std::cout << "[ServerNetworkManager] Server accept sent to client " << clientId << std::endl;
    }
}

void ServerNetworkManager::handleClientDisconnect(uint32_t clientId)
{
    std::cout << "[ServerNetworkManager] Client " << clientId << " disconnected" << std::endl;
    if (clientId < _clients.size()) {
        _clients[clientId].active = false;
        _clients[clientId].socket->close();
    }
}

void ServerNetworkManager::handleHeartbeat(uint32_t clientId, uint64_t currentMs)
{
    std::cout << "[ServerNetworkManager] Heartbeat from client " << clientId << std::endl;
    _clients[clientId].lastHeartbeatTime = currentMs;
}

void ServerNetworkManager::handlePing(const common::protocol::Packet& packet, uint32_t clientId)
{
    std::cout << "[ServerNetworkManager] Ping from client " << clientId << std::endl;
    
    // Send pong using PacketManager
    std::vector<uint8_t> args;
    args.push_back(0);  // flags_count
    // sequence_number (4 bytes, little-endian)
    uint32_t seq = packet.header.sequence_number;
    args.push_back(seq & 0xFF);
    args.push_back((seq >> 8) & 0xFF);
    args.push_back((seq >> 16) & 0xFF);
    args.push_back((seq >> 24) & 0xFF);
    // timestamp (4 bytes, little-endian)
    uint32_t ts = packet.header.timestamp;
    args.push_back(ts & 0xFF);
    args.push_back((ts >> 8) & 0xFF);
    args.push_back((ts >> 16) & 0xFF);
    args.push_back((ts >> 24) & 0xFF);
    // client_timestamp (4 bytes, little-endian)
    args.push_back(ts & 0xFF);
    args.push_back((ts >> 8) & 0xFF);
    args.push_back((ts >> 16) & 0xFF);
    args.push_back((ts >> 24) & 0xFF);
    
    auto pong = PacketManager::createPong(args);
    if (pong) {
        queueOutgoing(pong.value(), clientId);
    }
}

} // namespace network
} // namespace server
