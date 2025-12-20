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
#include <common/protocol/PacketManager.hpp>
#include <sstream>
#include <common/logger/Logger.hpp>
#include <engine/GameEngine.hpp>
#include <common/error/Error.hpp>
#include <common/constants/defines.hpp>

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
        LOG_ERROR("Failed to bind acceptor port {}", _basePort);
        throw Error(ErrorType::NetworkError, "Failed to bind server port " + std::to_string(_basePort));
    }
    LOG_INFO("ServerNetworkManager started on port {}", _basePort);
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

void ServerNetworkManager::queueOutgoing(const common::protocol::Packet& packet, std::optional<uint32_t> targetClient)
{
    std::lock_guard<std::mutex> lock(_outMutex);
    _outgoing.emplace_back(packet, targetClient);
    LOG_DEBUG("ServerNetworkManager: queued outgoing type={} target={} queue_size={}",
              static_cast<int>(packet.header.packet_type),
              targetClient.has_value() ? std::to_string(targetClient.value()) : std::string("broadcast"),
              _outgoing.size());
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
        // Receive packets from the acceptor socket
        common::protocol::Packet incoming;
        std::string remoteAddress;

        if (_acceptorSocket->receiveFrom(incoming, remoteAddress)) {
            LOG_DEBUG("Received packet from {}", remoteAddress);
            std::lock_guard<std::mutex> lock(_inMutex);
            auto clientId = findClientIdByAddress(remoteAddress);
            if (clientId.has_value()) {
                _incoming.push_back({incoming, clientId.value()});
                LOG_DEBUG("ServerNetworkManager: incoming queue size={}", _incoming.size());
            }
            if (!shouldForward(incoming)) {
                handleNetworkPacket(incoming, remoteAddress);
            }
        }

        {
            std::lock_guard<std::mutex> lock(_outMutex);
            LOG_DEBUG("ServerNetworkManager: processing outgoing queue size={}", _outgoing.size());
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
                LOG_DEBUG("ServerNetworkManager: sent outgoing type={} target={}", static_cast<int>(packet.header.packet_type), target.has_value() ? std::to_string(target.value()) : std::string("broadcast"));
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
        case protocol::PacketTypes::TYPE_CLIENT_DISCONNECT:
            return false;
        default:
            return true;
    }
}

void ServerNetworkManager::handleNetworkPacket(const common::protocol::Packet& packet, const std::string& remoteAddress)
{
    const auto type = static_cast<protocol::PacketTypes>(packet.header.packet_type);

    switch (type) {
        case protocol::PacketTypes::TYPE_CLIENT_CONNECT: {
            LOG_INFO("Client connection request received from {}", remoteAddress);
            handleClientConnect(remoteAddress);
            break;
        }

        case protocol::PacketTypes::TYPE_CLIENT_DISCONNECT: {
            LOG_INFO("Client disconnected from {}", remoteAddress);
            handleClientDisconnect(remoteAddress);
            break;
        }
        default: {
            LOG_ERROR("Unknown network packet type: {}", static_cast<int>(packet.header.packet_type));
            break;
        }
    }
}

void ServerNetworkManager::handleClientConnect(const std::string& remoteAddress)
{
    LOG_INFO("Client connecting from {}", remoteAddress);

    auto existingId = findClientIdByAddress(remoteAddress);
    if (existingId.has_value()) {
        LOG_INFO("Client reconnection from same address, updating...");
        uint32_t clientId = existingId.value();
        return;
    }

    // Find a free slot
    auto freeSlot = findFreeSlot();
    if (!freeSlot.has_value()) {
        LOG_WARN("No free slots for new client from {}", remoteAddress);
        // Send server reject (reason: server full = 0x01)
        std::vector<uint8_t> args;
        // flags_count = 1, FLAG_RELIABLE
        args.push_back(0x01);
        args.push_back(0x01);
        // sequence_number (4 bytes, little-endian) - use 0
        args.push_back(0); args.push_back(0); args.push_back(0); args.push_back(0);
        // timestamp (4 bytes, little-endian) - use 0
        args.push_back(0); args.push_back(0); args.push_back(0); args.push_back(0);
        // reject_code (1 byte) - 0x01 = server full
        args.push_back(0x01);
        // reason_message (64 bytes) - ASCII string, padded with zeros
        const std::string reason = "Server full";
        for (size_t i = 0; i < reason.size() && i < 64; ++i) {
            args.push_back(static_cast<uint8_t>(reason[i]));
        }
        for (size_t i = reason.size(); i < 64; ++i) {
            args.push_back(0);
        }
        auto reject = PacketManager::createServerReject(args);
        if (reject) {
            _acceptorSocket->sendTo(reject.value(), remoteAddress);
            LOG_INFO("Server reject sent to {}", remoteAddress);
        } else {
            LOG_ERROR("Failed to create ServerReject packet for {}", remoteAddress);
        }
        return;
    }

    uint32_t clientId = freeSlot.value();
    _clients[clientId].remoteAddress = remoteAddress;
    _clients[clientId].active = true;

    // Track the mapping
    _addressToClientId[remoteAddress] = clientId;

    LOG_INFO("Client {} assigned from {}", clientId, remoteAddress);

    // Send server accept to client using PacketManager
    std::vector<uint8_t> args;
    args.push_back(0x01);  // flags_count = 1
    args.push_back(0x01);  // FLAG_RELIABLE

    auto push32 = [&](uint32_t v) {
        args.push_back(static_cast<uint8_t>(v & 0xFF));
        args.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
        args.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
        args.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
    };

    // sequence_number (4 bytes, little-endian)
    uint32_t sequence_number = 0;
    push32(sequence_number);

    // timestamp (4 bytes, little-endian)
    uint32_t timestamp = static_cast<uint32_t>(TIMESTAMP & 0xFFFFFFFF);
    push32(timestamp);

    // assigned_player_id (4 bytes, little-endian)
    uint32_t id = clientId;
    push32(id);

    // max_players (1 byte)
    args.push_back(static_cast<uint8_t>(_maxPlayers & 0xFF));

    // game_instance_id (4 bytes, little-endian) - use 1 for now
    uint32_t game_instance_id = 1;
    push32(game_instance_id);

    // server_tickrate (2 bytes, little-endian)
    uint16_t tickrate = 60;
    args.push_back(static_cast<uint8_t>(tickrate & 0xFF));
    args.push_back(static_cast<uint8_t>((tickrate >> 8) & 0xFF));

    auto packet = PacketManager::createServerAccept(args);
    if (packet) {
        queueOutgoing(packet.value(), clientId);
        LOG_INFO("Server accept sent to client {}", clientId);
    } else {
        LOG_ERROR("Failed to create ServerAccept packet for client {}", clientId);
    }
}

void ServerNetworkManager::handleClientDisconnect(const std::string& remoteAddress)
{
    auto clientId = findClientIdByAddress(remoteAddress);
    if (clientId.has_value()) {
        uint32_t id = clientId.value();
        LOG_INFO("Client {} disconnected from {}", id, remoteAddress);
        if (id < _clients.size()) {
            _clients[id].active = false;
            _addressToClientId.erase(remoteAddress);
        }
    }
}

} // namespace network
} // namespace server
