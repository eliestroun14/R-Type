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
        return;
    }
    _socket->setNonBlocking(true);
    _socket->bind(0);  // Bind on any available port
    _socket->connect(_host, _port);  // Connect to server
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
    std::cout << "[ClientNetworkManager] Waiting for server connection..." << std::endl;
    while (!_connected && _running.load()) {
        common::protocol::Packet incoming;
        std::string remoteAddress;
        if (_socket->receiveFrom(incoming, remoteAddress)) {
            std::cout << "[ClientNetworkManager] Phase 1: Received packet type: " << (int)incoming.header.packet_type << std::endl;
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
        std::cout << "[ClientNetworkManager] Connection failed, stopping" << std::endl;
        return;
    }

    std::cout << "[ClientNetworkManager] Connected! Starting normal operation" << std::endl;

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
            std::cout << "[ClientNetworkManager] Connection accepted by server" << std::endl;
            handleConnectionAccepted(packet);
            break;
        }

        case protocol::PacketTypes::TYPE_SERVER_REJECT: {
            std::cout << "[ClientNetworkManager] Connection rejected by server" << std::endl;
            handleConnectionRejected(packet);
            break;
        }

        case protocol::PacketTypes::TYPE_ACK: {
            std::cout << "[ClientNetworkManager] ACK received from server" << std::endl;
            handleAck(packet);
            break;
        }

        case protocol::PacketTypes::TYPE_PING: {
            std::cout << "[ClientNetworkManager] Ping received from server" << std::endl;
            handlePing(packet);
            break;
        }

        case protocol::PacketTypes::TYPE_PONG: {
            std::cout << "[ClientNetworkManager] Pong received from server" << std::endl;
            handlePong(packet);
            break;
        }

        case protocol::PacketTypes::TYPE_CLIENT_DISCONNECT: {
            handleDisconnect(packet);
            break;
        }

        default:
            std::cerr << "[ClientNetworkManager] Unknown network packet type: " 
                      << static_cast<int>(packet.header.packet_type) << std::endl;
            break;
    }
}

void ClientNetworkManager::handleConnectionAccepted(const common::protocol::Packet& packet)
{
    std::cout << "[handleConnectionAccepted] Connection accepted by server" << std::endl;
    std::cout << "[handleConnectionAccepted] Packet data size: " << packet.data.size() << std::endl;
    protocol::ServerAccept payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));
    std::cout << "[handleConnectionAccepted] Assigned player ID: " << payload.assigned_player_id << std::endl;
    _client->setSelfId(payload.assigned_player_id);
    _connected = true;
    _client->setConnected(true);
    std::cout << "[handleConnectionAccepted] Client connected set to true" << std::endl;
}

void ClientNetworkManager::handleConnectionRejected(const common::protocol::Packet& packet)
{
    std::cout << "[handleConnectionRejected] Connection rejected by server" << std::endl;
    protocol::ServerReject payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));
    std::cerr << "Reject reason: " << payload.reason_message << std::endl;
    _client->setConnected(false);
    _client->stop();
}

void ClientNetworkManager::handleDisconnect(const common::protocol::Packet& packet)
{
    std::cout << "[handleDisconnect] Disconnected by server" << std::endl;
    protocol::ClientDisconnect payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));
    std::cerr << "Disconnect reason: " << static_cast<int>(payload.reason) << std::endl;
    _client->stop();
}

void ClientNetworkManager::handleAck(const common::protocol::Packet& packet)
{
    std::cout << "[handleAck] ACK received from server" << std::endl;
    protocol::Acknowledgment payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));
    // TODO we will prob have a ACK queue to manage reliable packets
}

void ClientNetworkManager::handlePong(const common::protocol::Packet& packet)
{
    std::cout << "[handlePong] Pong received from server" << std::endl;
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
    std::cout << "[ClientNetworkManager] Connection request sent" << std::endl;
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
    std::cout << "[ClientNetworkManager] Heartbeat sent" << std::endl;
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
    std::cout << "[ClientNetworkManager] Disconnect packet sent" << std::endl;
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
    std::cout << "[ClientNetworkManager] ACK packet sent" << std::endl;
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
    std::cout << "[ClientNetworkManager] Ping packet sent" << std::endl;
}

void ClientNetworkManager::handlePing(const common::protocol::Packet& packet)
{
    std::cout << "[ClientNetworkManager] Ping received from server, sending pong" << std::endl;
    
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
    std::cout << "[ClientNetworkManager] Pong packet sent" << std::endl;
}

} // namespace network
} // namespace client
