/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ClientNetworkManager
*/

#include <client/network/ClientNetworkManager.hpp>
#include <client/RTypeClient.hpp>
#include <common/protocol/Protocol.hpp>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

namespace client {
namespace network {

ClientNetworkManager::ClientNetworkManager(const std::string& host, uint16_t port, RTypeClient* client)
    : _host(host), _port(port), _socket(std::make_shared<common::network::AsioSocket>()), _running(false), _client(client)
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
    _socket->connect(_host, _port);
    _thread = std::thread(&ClientNetworkManager::networkLoop, this);
}

void ClientNetworkManager::stop()
{
    if (!_running.exchange(false)) {
        return;
    }
    if (_thread.joinable()) {
        _thread.join();
    }
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

void ClientNetworkManager::networkLoop()
{
    while (_running.load()) {
        common::protocol::Packet incoming;
        if (_socket->receive(incoming)) {
            if (shouldForward(incoming)) {
                std::lock_guard<std::mutex> lock(_inMutex);
                _incoming.push_back({incoming, std::nullopt});
            } else {
                // Handle network-level packets (connection, heartbeat, etc.)
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

            // Extract assigned player ID from packet data and notify client
            protocol::ServerAccept payload;
            std::memcpy(&payload, packet.data.data(), sizeof(payload));
            _client->setSelfId(payload.assigned_player_id);
            break;
        }

        case protocol::PacketTypes::TYPE_SERVER_REJECT: {
            std::cout << "[ClientNetworkManager] Connection rejected by server" << std::endl;
            // TODO: Extract reject reason and notify client
            protocol::ServerReject payload;
            std::memcpy(&payload, packet.data.data(), sizeof(payload));
            std::cerr << "Reject reason: " << payload.reason_message << std::endl;
            _client->stop();
            break;
        }

        case protocol::PacketTypes::TYPE_ACK: {
            protocol::Acknowledgment payload;
            std::memcpy(&payload, packet.data.data(), sizeof(payload));
            //_client->onAckReceived(payload);
            break;
        }
        
        case protocol::PacketTypes::TYPE_CLIENT_CONNECT: {
            break;
        }

        case protocol::PacketTypes::TYPE_CLIENT_DISCONNECT: {
            // Server disconnects client
            std::cout << "[ClientNetworkManager] Disconnected by server" << std::endl;
            protocol::ClientDisconnect payload;
            std::memcpy(&payload, packet.data.data(), sizeof(payload));
            std::cerr << "Disconnect reason: " << static_cast<int>(payload.reason) << std::endl;
            _client->stop();
            break;
        }

        default:
            std::cerr << "[ClientNetworkManager] Unknown network packet type: " 
                      << static_cast<int>(packet.header.packet_type) << std::endl;
            break;
    }
}

} // namespace network
} // namespace client
