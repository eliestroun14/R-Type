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
#include <sstream>

using namespace std::chrono_literals;

namespace server {
namespace network {

ServerNetworkManager::ServerNetworkManager(uint16_t basePort, uint32_t maxPlayers)
    : _basePort(basePort),
      _maxPlayers(std::min<uint32_t>(maxPlayers, MAX_PLAYERS)),
      _clients(_maxPlayers),
      _running(false)
{
    _acceptorSocket = std::make_shared<common::network::AsioSocket>();
    for (uint32_t i = 0; i < _maxPlayers; ++i) {
        _clients[i].clientId = i;
        _clients[i].active = false;
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

    _acceptorSocket->setNonBlocking(true);
    if (!_acceptorSocket->bind(_basePort)) {
        std::cerr << "[ServerNetworkManager] Failed to bind acceptor port " << _basePort << std::endl;
    }
}

void ServerNetworkManager::stop()
{
    if (!_running.exchange(false)) {
        return;
    }
    if (_acceptorSocket) {
        _acceptorSocket->close();
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

std::optional<uint32_t> ServerNetworkManager::findClientIdByAddress(const std::string& remoteAddress)
{
    auto it = _addressToClientId.find(remoteAddress);
    if (it != _addressToClientId.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<uint32_t> ServerNetworkManager::findFreeSlot()
{
    for (uint32_t i = 0; i < _maxPlayers; ++i) {
        if (!_clients[i].active) {
            return i;
        }
    }
    return std::nullopt;
}

void ServerNetworkManager::run()
{
    while (_running.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        uint64_t currentMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime.time_since_epoch()).count();

        // Check for client timeouts
        checkClientTimeouts();

        // Receive packets from the acceptor socket
        common::protocol::Packet incoming;
        std::string remoteAddress;
        
        if (_acceptorSocket->receiveFrom(incoming, remoteAddress)) {
            std::cout << "[ServerNetworkManager] Received packet from " << remoteAddress << std::endl;
            if (shouldForward(incoming)) {
                std::lock_guard<std::mutex> lock(_inMutex);
                auto clientId = findClientIdByAddress(remoteAddress);
                if (clientId.has_value()) {
                    _incoming.push_back({incoming, clientId.value()});
                }
            } else {
                // Handle network-level packets (connection, heartbeat, etc.)
                handleNetworkPacket(incoming, remoteAddress, currentMs);
            }
        }

        {
            std::lock_guard<std::mutex> lock(_outMutex);
            while (!_outgoing.empty()) {
                const auto [packet, target] = _outgoing.front();
                if (target.has_value()) {
                    uint32_t idx = target.value();
                    if (idx < _clients.size() && _clients[idx].active) {
                        _acceptorSocket->sendTo(packet, _clients[idx].remoteAddress);
                    }
                } else {
                    for (auto& slot : _clients) {
                        if (slot.active) {
                            _acceptorSocket->sendTo(packet, slot.remoteAddress);
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
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    for (uint32_t i = 0; i < _maxPlayers; ++i) {
        auto& slot = _clients[i];
        if (slot.active && slot.lastHeartbeatTime > 0) {
            uint64_t timeSinceLastHeartbeat = currentTime - slot.lastHeartbeatTime;
            if (timeSinceLastHeartbeat > TIMEOUT_MS) {
                std::cout << "[ServerNetworkManager] Client " << i << " timeout (no heartbeat for " 
                          << timeSinceLastHeartbeat << "ms)" << std::endl;
                slot.active = false;
                _addressToClientId.erase(slot.remoteAddress);
            }
        }
    }
}

void ServerNetworkManager::handleNetworkPacket(const common::protocol::Packet& packet, const std::string& remoteAddress, uint64_t currentMs)
{
    const auto type = static_cast<protocol::PacketTypes>(packet.header.packet_type);
    
    switch (type) {
        case protocol::PacketTypes::TYPE_CLIENT_CONNECT: {
            std::cout << "[ServerNetworkManager] Client connection request received from " << remoteAddress << std::endl;
            handleClientConnect(remoteAddress, currentMs);
            break;
        }
        
        case protocol::PacketTypes::TYPE_CLIENT_DISCONNECT: {
            std::cout << "[ServerNetworkManager] Client disconnected from " << remoteAddress << std::endl;
            handleClientDisconnect(remoteAddress);
            break;
        }
        
        case protocol::PacketTypes::TYPE_HEARTBEAT: {
            auto clientId = findClientIdByAddress(remoteAddress);
            if (clientId.has_value()) {
                std::cout << "[ServerNetworkManager] Heartbeat received from client " << clientId.value() << std::endl;
                handleHeartbeat(remoteAddress, currentMs);
            }
            break;
        }
        
        case protocol::PacketTypes::TYPE_PING: {
            auto clientId = findClientIdByAddress(remoteAddress);
            if (clientId.has_value()) {
                std::cout << "[ServerNetworkManager] Ping received from client " << clientId.value() << std::endl;
                handlePing(packet, remoteAddress);
            }
            break;
        }
        
        case protocol::PacketTypes::TYPE_ACK:
        case protocol::PacketTypes::TYPE_PONG:
            // Acknowledgment received, no action needed
            break;
        
        case protocol::PacketTypes::TYPE_SERVER_ACCEPT:
        case protocol::PacketTypes::TYPE_SERVER_REJECT:
            // These should only be sent by server, not received
            std::cerr << "[ServerNetworkManager] Unexpected packet type from client at " << remoteAddress << std::endl;
            break;
        
        default:
            std::cerr << "[ServerNetworkManager] Unknown network packet type: " 
                      << static_cast<int>(packet.header.packet_type) << std::endl;
            break;
    }
}

void ServerNetworkManager::handleClientConnect(const std::string& remoteAddress, uint64_t currentMs)
{
    std::cout << "[ServerNetworkManager] Client connecting from " << remoteAddress << std::endl;
    
    // Check if this client is already connected
    auto existingId = findClientIdByAddress(remoteAddress);
    if (existingId.has_value()) {
        std::cout << "[ServerNetworkManager] Client reconnection from same address, updating..." << std::endl;
        uint32_t clientId = existingId.value();
        _clients[clientId].lastHeartbeatTime = currentMs;
        return;
    }
    
    // Find a free slot
    auto freeSlot = findFreeSlot();
    if (!freeSlot.has_value()) {
        std::cerr << "[ServerNetworkManager] No free slots for new client" << std::endl;
        return;
    }
    
    uint32_t clientId = freeSlot.value();
    _clients[clientId].remoteAddress = remoteAddress;
    _clients[clientId].lastHeartbeatTime = currentMs;
    _clients[clientId].active = true;
    
    // Track the mapping
    _addressToClientId[remoteAddress] = clientId;
    
    std::cout << "[ServerNetworkManager] Client " << clientId << " assigned from " << remoteAddress << std::endl;
    
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

void ServerNetworkManager::handleClientDisconnect(const std::string& remoteAddress)
{
    auto clientId = findClientIdByAddress(remoteAddress);
    if (clientId.has_value()) {
        uint32_t id = clientId.value();
        std::cout << "[ServerNetworkManager] Client " << id << " disconnected" << std::endl;
        if (id < _clients.size()) {
            _clients[id].active = false;
            _addressToClientId.erase(remoteAddress);
        }
    }
}

void ServerNetworkManager::handleHeartbeat(const std::string& remoteAddress, uint64_t currentMs)
{
    auto clientId = findClientIdByAddress(remoteAddress);
    if (clientId.has_value()) {
        _clients[clientId.value()].lastHeartbeatTime = currentMs;
    }
}

void ServerNetworkManager::handlePing(const common::protocol::Packet& packet, const std::string& remoteAddress)
{
    auto clientId = findClientIdByAddress(remoteAddress);
    if (!clientId.has_value()) {
        return;
    }
    
    std::cout << "[ServerNetworkManager] Ping from client " << clientId.value() << std::endl;
    
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
        queueOutgoing(pong.value(), clientId.value());
    }
}

} // namespace network
} // namespace server
