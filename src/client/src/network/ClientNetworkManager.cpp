/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ClientNetworkManager
*/

#include <client/network/ClientNetworkManager.hpp>
#include <client/RTypeClient.hpp>
#include <common/protocol/Protocol.hpp>
#include <common/protocol/Payload.hpp>
#include <common/constants/defines.hpp>
#include <chrono>
#include <iostream>
#include <cstring>
#include <thread>
#include <common/logger/Logger.hpp>
#include <common/error/Error.hpp>

using namespace std::chrono_literals;

namespace client {
namespace network {

ClientNetworkManager::ClientNetworkManager(const std::string& host, uint16_t port, RTypeClient* client)
    : _host(host), _port(port), _socket(std::make_shared<common::network::AsioSocket>()), _running(false), _connected(false), _client(client)
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

void ClientNetworkManager::queueOutgoing(const common::protocol::Packet& packet, std::optional<uint32_t>)
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
        case protocol::PacketTypes::TYPE_SERVER_ACCEPT:
        case protocol::PacketTypes::TYPE_SERVER_REJECT:
        case protocol::PacketTypes::TYPE_CLIENT_DISCONNECT:
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
            LOG_DEBUG("Phase 1: Received packet type: {} from {}", static_cast<int>(incoming.header.packet_type), remoteAddress);
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
        common::protocol::Packet incoming;
        std::string remoteAddress;
        if (_socket->receiveFrom(incoming, remoteAddress)) {
                LOG_DEBUG("Phase 2: Received packet type: {} from {}", static_cast<int>(incoming.header.packet_type), remoteAddress);
                if (shouldForward(incoming)) {
                    std::lock_guard<std::mutex> lock(_inMutex);
                    _incoming.push_back({incoming, std::nullopt});
                    LOG_DEBUG("Queued incoming packet type={} (forwarded)", static_cast<int>(incoming.header.packet_type));
                } else {
                    LOG_DEBUG("Handling network packet type={} (control)", static_cast<int>(incoming.header.packet_type));
                    handleNetworkPacket(incoming);
                }
        }

        {
            std::lock_guard<std::mutex> lock(_outMutex);
            while (!_outgoing.empty()) {
                LOG_DEBUG("Sending outgoing packet type={}", static_cast<int>(_outgoing.front().header.packet_type));
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

    protocol::ServerAcceptPayload payload;

    if (packet.data.size() < sizeof(payload)) {
        LOG_ERROR("Invalid ServerAccept packet size: expected {}, got {}", sizeof(payload), packet.data.size());
        return;
    }

    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO("Assigned player ID: {}", payload.assigned_player_id);
    _connected = true;
    _client->setConnected(true);
    LOG_DEBUG("Client connected status set to true");
}

void ClientNetworkManager::handleConnectionRejected(const common::protocol::Packet& packet)
{
    LOG_ERROR("Connection rejected by server");

    protocol::ServerRejectPayload payload;

    if (packet.data.size() < sizeof(payload)) {
        LOG_ERROR("Invalid ServerReject packet size: expected {}, got {}", sizeof(payload), packet.data.size());
        return;
    }

    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_ERROR("Reject code: {}, reason: {}", static_cast<int>(payload.reject_code), payload.reason_message);
    _client->setConnected(false);
}

void ClientNetworkManager::handleDisconnect(const common::protocol::Packet& packet)
{
    LOG_WARN("Disconnected by server");

    protocol::ClientDisconnectPayload payload;

    if (packet.data.size() < sizeof(payload)) {
        LOG_ERROR("Invalid ClientDisconnect packet size: expected {}, got {}", sizeof(payload), packet.data.size());
        return;
    }

    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_WARN("Disconnect reason: {}", static_cast<int>(payload.reason));
    _client->setConnected(false);
}

void ClientNetworkManager::sendConnectionRequest()
{
    common::protocol::PacketHeader header;
    header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_CONNECT);
    header.sequence_number = 0;
    header.timestamp = TIMESTAMP;
    header.flags = 0;

    protocol::ClientConnect payload;
    payload.header = header;
    payload.protocol_version = PROTOCOL_VERSION;
    std::strncpy(payload.player_name, _client->getPlayerName().c_str(), sizeof(payload.player_name) - 1);
    payload.client_id = 0;

    common::protocol::Packet connect(header, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&payload), reinterpret_cast<uint8_t*>(&payload) + sizeof(payload)));
    queueOutgoing(connect);
    LOG_INFO("Connection request sent to server");
}

} // namespace network
} // namespace client
