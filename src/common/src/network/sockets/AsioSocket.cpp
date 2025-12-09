/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** AsioSocket
*/

#include "../../../include/common/network/sockets/AsioSocket.hpp"
#include <iostream>

namespace common {
namespace network {

AsioSocket::AsioSocket()
    : ASocket(),
      _ioContext(std::make_unique<asio::io_context>()),
      _socket(nullptr),
      _maxPacketSize(65507)
{
    _receiveBuffer.resize(_maxPacketSize);
}

AsioSocket::AsioSocket(const std::string& host, uint16_t port)
    : AsioSocket()
{
    connect(host, port);
}

AsioSocket::~AsioSocket()
{
    close();
}

void AsioSocket::initializeSocket()
{
    try {
        if (!_socket) {
            _socket = std::make_unique<asio::ip::udp::socket>(*_ioContext);
            _socket->open(asio::ip::udp::v4());
            // Set socket to non-blocking mode
            if (_nonBlocking) {
                _socket->non_blocking(true);
            }
        }
    } catch (const std::exception& e) {
        setError(std::string("Failed to initialize socket: ") + e.what());
    }
}

asio::ip::udp::endpoint AsioSocket::resolveEndpoint(const std::string& host, uint16_t port)
{
    try {
        asio::ip::udp::resolver resolver(*_ioContext);
        asio::ip::udp::resolver::query query(asio::ip::udp::v4(), host, std::to_string(port));
        asio::ip::udp::resolver::iterator iter = resolver.resolve(query);
        
        if (iter != asio::ip::udp::resolver::iterator()) {
            return *iter;
        }
    } catch (const std::exception& e) {
        setError(std::string("Failed to resolve endpoint: ") + e.what());
    }
    
    return asio::ip::udp::endpoint();
}

bool AsioSocket::connect(const std::string& host, uint16_t port)
{
    try {
        if (!_socket) {
            initializeSocket();
        }

        if (!_socket) {
            setError("Socket not initialized");
            return false;
        }

        _remoteEndpoint = resolveEndpoint(host, port);
        
        if (_remoteEndpoint == asio::ip::udp::endpoint()) {
            setError("Failed to resolve remote endpoint");
            return false;
        }

        _remoteAddress = host;
        _remotePort = port;
        setConnectionState(true);

        return true;
    } catch (const std::exception& e) {
        setError(std::string("Connection failed: ") + e.what());
        setConnectionState(false);
        return false;
    }
}

bool AsioSocket::bind(uint16_t port)
{
    try {
        if (!_socket) {
            initializeSocket();
        }

        if (!_socket) {
            setError("Socket not initialized");
            return false;
        }

        asio::ip::udp::endpoint endpoint(asio::ip::udp::v4(), port);
        _socket->bind(endpoint);
        
        _localPort = port;
        setConnectionState(true);

        return true;
    } catch (const std::exception& e) {
        setError(std::string("Bind failed: ") + e.what());
        return false;
    }
}

bool AsioSocket::send(const protocol::Packet& packet)
{
    if (!isConnected() || !_socket) {
        setError("Socket not connected");
        return false;
    }

    if (!validatePacket(packet)) {
        setError("Invalid packet");
        return false;
    }

    try {
        std::vector<uint8_t> buffer;
        packet.serialize(buffer);

        std::error_code ec;
        size_t bytesSent = _socket->send_to(asio::buffer(buffer), _remoteEndpoint, 0, ec);

        if (ec) {
            setError(std::string("Send failed: ") + ec.message());
            return false;
        }

        return bytesSent > 0;
    } catch (const std::exception& e) {
        setError(std::string("Send exception: ") + e.what());
        return false;
    }
}

bool AsioSocket::receive(protocol::Packet& packet)
{
    if (!_socket) {
        return false;
    }

    try {
        // Poll for available data
        if (!hasData()) {
            return false;
        }

        std::error_code ec;
        size_t bytesReceived = _socket->receive_from(
            asio::buffer(_receiveBuffer),
            _senderEndpoint,
            0,
            ec
        );

        if (ec) {
            if (ec == asio::error::would_block || ec == asio::error::try_again) {
                // No data available, not an error
                return false;
            }
            setError(std::string("Receive failed: ") + ec.message());
            return false;
        }

        if (bytesReceived == 0) {
            return false;
        }

        // Deserialize the received data into a packet
        std::vector<uint8_t> data(_receiveBuffer.begin(), _receiveBuffer.begin() + bytesReceived);
        if (!packet.deserialize(data)) {
            setError("Failed to deserialize packet");
            return false;
        }

        // Update remote endpoint info if this is the first packet
        if (_remoteAddress.empty()) {
            _remoteAddress = _senderEndpoint.address().to_string();
            _remotePort = _senderEndpoint.port();
        }

        return true;
    } catch (const std::exception& e) {
        setError(std::string("Receive exception: ") + e.what());
        return false;
    }
}

void AsioSocket::close()
{
    if (_socket && _socket->is_open()) {
        try {
            _socket->close();
        } catch (const std::exception& e) {
            setError(std::string("Close failed: ") + e.what());
        }
    }
    
    setConnectionState(false);
}

bool AsioSocket::hasData() const
{
    if (!_socket || !_socket->is_open()) {
        return false;
    }

    try {
        std::error_code ec;
        size_t available = _socket->available(ec);
        
        if (ec) {
            return false;
        }
        
        return available > 0;
    } catch (const std::exception&) {
        return false;
    }
}

void AsioSocket::setBufferSizes(size_t sendBufferSize, size_t receiveBufferSize)
{
    if (_socket && _socket->is_open()) {
        try {
            asio::socket_base::send_buffer_size sendOption(sendBufferSize);
            asio::socket_base::receive_buffer_size receiveOption(receiveBufferSize);
            
            _socket->set_option(sendOption);
            _socket->set_option(receiveOption);
        } catch (const std::exception& e) {
            setError(std::string("Failed to set buffer sizes: ") + e.what());
        }
    }
}

void AsioSocket::setBroadcast(bool enable)
{
    if (_socket && _socket->is_open()) {
        try {
            asio::socket_base::broadcast option(enable);
            _socket->set_option(option);
        } catch (const std::exception& e) {
            setError(std::string("Failed to set broadcast: ") + e.what());
        }
    }
}

void AsioSocket::setMaxPacketSize(size_t maxSize)
{
    _maxPacketSize = std::min(maxSize, size_t(65507));
    _receiveBuffer.resize(_maxPacketSize);
}

} // namespace network
} // namespace common
