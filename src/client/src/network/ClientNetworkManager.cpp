/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ClientNetworkManager
*/

#include <client/network/ClientNetworkManager.hpp>
#include <client/RTypeClient.hpp>
#include <common/protocol/Protocol.hpp>
#include <common/constants/defines.hpp>
#include <chrono>
#include <iostream>
#include <cstring>
#include <thread>
#include <engine/utils/Logger.hpp>
#include <common/error/Error.hpp>

using namespace std::chrono_literals;

namespace client {
namespace network {

ClientNetworkManager::ClientNetworkManager(const std::string& host, uint16_t port, RTypeClient* client)
    : _host(host), _port(port), _socket(std::make_shared<common::network::AsioSocket>()), _running(false), _connected(false), _client(client), _tickCount(0)
{
}

ClientNetworkManager::~ClientNetworkManager()
{
    stop();
}

void ClientNetworkManager::start()
{
    if (_running.exchange(true)) {
        LOG_WARN("ClientNetworkManager already running");
        return;
    }
    _socket->setNonBlocking(true);
    _socket->bind(0);  // Bind on any available port
    _socket->connect(_host, _port);  // Connect to server
    LOG_INFO("ClientNetworkManager started, connecting to {}:{}", _host, _port);
    // Thread is now managed by RTypeClient, not here
}

void ClientNetworkManager::stop()
{
    if (!_running.exchange(false)) {
        return;
    }
    // Thread is now managed by RTypeClient, so we don't join here
    _socket->close();
}

void ClientNetworkManager::queueOutgoing(const common::protocol::Packet& packet,
                                         std::optional<uint32_t>)
{
    std::lock_guard<std::mutex> lock(_outMutex);
    _outgoing.push_back(packet);
}

std::vector<common::network::ReceivedPacket> ClientNetworkManager::fetchIncoming()
{
    std::lock_guard<std::mutex> lock(_inMutex);
    std::vector<common::network::ReceivedPacket> packets(_incoming.begin(), _incoming.end());
    _incoming.clear();
    return packets;
}

bool ClientNetworkManager::shouldForward(const common::protocol::Packet& packet) const
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

void ClientNetworkManager::run()
{
    // Phase 1: Wait for server connection response
    LOG_INFO("Waiting for server connection response...");
    while (!_connected && _running.load()) {
        common::protocol::Packet incoming;
        std::string remoteAddress;
        if (_socket->receiveFrom(incoming, remoteAddress)) {
            LOG_DEBUG("Phase 1: Received packet type: {}", static_cast<int>(incoming.header.packet_type));
            handleNetworkPacket(incoming);
        }
    
        std::lock_guard<std::mutex> lock(_outMutex);
        while (!_outgoing.empty()) {
            _socket->send(_outgoing.front());
            _outgoing.pop_front();
        }
    
        std::this_thread::sleep_for(1ms);
    }

    if (!_connected) {
        LOG_ERROR("Connection failed, stopping network manager");
        return;
    }

    LOG_INFO("Connected! Starting normal operation");

    // Phase 2: Normal operation (heartbeats, game packets, etc.)
    auto startEpoch = std::chrono::steady_clock::now();
    uint64_t lastHeartbeatTime = 0;

    while (_running.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        uint64_t elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startEpoch).count();

        if (elapsedMs - lastHeartbeatTime >= HEARTBEAT_INTERVAL) {
            sendHeartbeat();
            lastHeartbeatTime = elapsedMs;
        }

        common::protocol::Packet incoming;
        std::string remoteAddress;
        if (_socket->receiveFrom(incoming, remoteAddress)) {
            if (shouldForward(incoming)) {
                std::lock_guard<std::mutex> lock(_inMutex);
                _incoming.push_back({incoming, std::nullopt});
            } else {
                handleNetworkPacket(incoming);
            }
        }

        {
            std::lock_guard<std::mutex> lock(_outMutex);
            while (!_outgoing.empty()) {
                _socket->send(_outgoing.front());
                _outgoing.pop_front();
            }
        }

        std::this_thread::sleep_for(1ms);
    }
}

void ClientNetworkManager::handleNetworkPacket(const common::protocol::Packet& packet)
{
    const auto type = static_cast<protocol::PacketTypes>(packet.header.packet_type);

    switch (type) {
        case protocol::PacketTypes::TYPE_SERVER_ACCEPT: {
            handleConnectionAccepted(packet);
            break;
        }

        case protocol::PacketTypes::TYPE_SERVER_REJECT: {
            LOG_WARN("Connection rejected by server");
            handleConnectionRejected(packet);
            break;
        }

        case protocol::PacketTypes::TYPE_ACK: {
            LOG_DEBUG("ACK received from server");
            handleAck(packet);
            break;
        }

        case protocol::PacketTypes::TYPE_PING: {
            LOG_DEBUG("Ping received from server");
            handlePing(packet);
            break;
        }

        case protocol::PacketTypes::TYPE_PONG: {
            LOG_DEBUG("Pong received from server");
            handlePong(packet);
            break;
        }

        case protocol::PacketTypes::TYPE_CLIENT_DISCONNECT: {
            handleDisconnect(packet);
            break;
        }

        default:
            LOG_ERROR("Unknown network packet type: {}", static_cast<int>(packet.header.packet_type));
            break;
    }
}

void ClientNetworkManager::handleConnectionAccepted(const common::protocol::Packet& packet)
{
    LOG_INFO("Connection accepted by server");
    LOG_DEBUG("Packet data size: {}", packet.data.size());
    protocol::ServerAccept payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));
    LOG_INFO("Assigned player ID: {}", payload.assigned_player_id);
    _client->setSelfId(payload.assigned_player_id);
    _connected = true;
    _client->setConnected(true);
    LOG_DEBUG("Client connected status set to true");
}

void ClientNetworkManager::handleConnectionRejected(const common::protocol::Packet& packet)
{
    LOG_ERROR("Connection rejected by server");
    protocol::ServerReject payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));
    LOG_ERROR("Reject reason: {}", payload.reason_message);
    _client->setConnected(false);
    _client->stop();
}

void ClientNetworkManager::handleDisconnect(const common::protocol::Packet& packet)
{
    LOG_WARN("Disconnected by server");
    protocol::ClientDisconnect payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));
    LOG_WARN("Disconnect reason: {}", static_cast<int>(payload.reason));
    _client->stop();
}

void ClientNetworkManager::handleAck(const common::protocol::Packet& packet)
{
    LOG_DEBUG("ACK received from server");
    protocol::Acknowledgment payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));
    // TODO we will prob have a ACK queue to manage reliable packets
}

void ClientNetworkManager::handlePong(const common::protocol::Packet& packet)
{
    LOG_DEBUG("Pong received from server");
    protocol::Pong payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));
}

void ClientNetworkManager::sendConnectionRequest()
{
    common::protocol::PacketHeader header;
    header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_CONNECT);
    header.sequence_number = _tickCount;
    header.timestamp = TIMESTAMP;
    header.flags = 0;

    protocol::ClientConnect payload;
    payload.header = header;
    payload.protocol_version = PROTOCOL_VERSION;
    std::strncpy(payload.player_name, _client->getPlayerName().c_str(), sizeof(payload.player_name) - 1);
    payload.client_id = 0; // 0 for initial connection

    common::protocol::Packet connect(header, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&payload), reinterpret_cast<uint8_t*>(&payload) + sizeof(payload)));
    queueOutgoing(connect);
    LOG_INFO("Connection request sent to server");
}

void ClientNetworkManager::sendHeartbeat()
{
    common::protocol::PacketHeader header;
    header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEARTBEAT);
    header.sequence_number = _tickCount;
    header.timestamp = TIMESTAMP;
    header.flags = 0;

    protocol::ClientConnect payload;
    payload.header = header;
    payload.protocol_version = PROTOCOL_VERSION;
    std::strncpy(payload.player_name, _client->getPlayerName().c_str(), sizeof(payload.player_name) - 1);
    payload.player_name[sizeof(payload.player_name) - 1] = '\0'; // Ensure null-termination
    payload.client_id = _client->getSelfId();

    common::protocol::Packet heartbeat(header, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&payload), reinterpret_cast<uint8_t*>(&payload) + sizeof(payload)));
    queueOutgoing(heartbeat);
    LOG_INFO("[ClientNetworkManager] Heartbeat sent");
}

void ClientNetworkManager::sendDisconnect(uint8_t reason)
{
    common::protocol::PacketHeader header;
    header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_DISCONNECT);
    header.sequence_number = _tickCount;
    header.timestamp = TIMESTAMP;
    header.flags = 0;

    protocol::ClientDisconnect payload;
    payload.header = header;
    payload.client_id = _client->getSelfId();
    payload.reason = reason;

    common::protocol::Packet disconnect(header, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&payload), reinterpret_cast<uint8_t*>(&payload) + sizeof(payload)));
    queueOutgoing(disconnect);
    LOG_INFO("[ClientNetworkManager] Disconnect packet sent");
}

void ClientNetworkManager::sendAck(uint32_t acked_sequence, uint32_t received_timestamp)
{
    common::protocol::PacketHeader header;
    header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_ACK);
    header.sequence_number = _tickCount;
    header.timestamp = TIMESTAMP;
    header.flags = 0;

    protocol::Acknowledgment payload;
    payload.header = header;
    payload.client_id = _client->getSelfId();
    payload.acked_sequence = acked_sequence;
    payload.received_timestamp = received_timestamp;

    common::protocol::Packet ack(header, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&payload), reinterpret_cast<uint8_t*>(&payload) + sizeof(payload)));
    queueOutgoing(ack);
    LOG_INFO("[ClientNetworkManager] ACK packet sent");
}

void ClientNetworkManager::sendPing()
{
    common::protocol::PacketHeader header;
    header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_PING);
    header.sequence_number = _tickCount;
    header.timestamp = TIMESTAMP;
    header.flags = 0;

    protocol::Ping payload;
    payload.header = header;
    payload.client_id = _client->getSelfId();
    payload.client_timestamp = TIMESTAMP;

    common::protocol::Packet ping(header, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&payload), reinterpret_cast<uint8_t*>(&payload) + sizeof(payload)));
    queueOutgoing(ping);
    LOG_INFO("[ClientNetworkManager] Ping packet sent");
}

void ClientNetworkManager::handlePing(const common::protocol::Packet& packet)
{
    LOG_INFO("[ClientNetworkManager] Ping received from server, sending pong");

    // Send pong response with same sequence number and timestamp
    common::protocol::PacketHeader header;
    header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_PONG);
    header.sequence_number = packet.header.sequence_number;
    header.timestamp = packet.header.timestamp;
    header.flags = 0;

    protocol::Pong payload;
    payload.header = header;
    payload.client_timestamp = TIMESTAMP;

    common::protocol::Packet pong(header, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&payload), reinterpret_cast<uint8_t*>(&payload) + sizeof(payload)));
    queueOutgoing(pong);
    LOG_INFO("[ClientNetworkManager] Pong packet sent");
}

} // namespace network
} // namespace client
