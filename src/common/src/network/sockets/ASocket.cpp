/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ASocket
*/

#include "../../../include/common/network/sockets/ASocket.hpp"
#include <algorithm>

namespace common {
namespace network {

ASocket::ASocket()
    : _connected(false),
      _nonBlocking(true),
      _localPort(0),
      _remotePort(0),
      _remoteAddress(""),
      _lastError(""),
      _timeout(5000)
{
}

ASocket::~ASocket()
{
    _connected = false;
}

bool ASocket::isConnected() const
{
    return _connected.load();
}

uint16_t ASocket::getLocalPort() const
{
    return _localPort;
}

std::string ASocket::getRemoteAddress() const
{
    return _remoteAddress;
}

uint16_t ASocket::getRemotePort() const
{
    return _remotePort;
}

void ASocket::setNonBlocking(bool nonBlocking)
{
    _nonBlocking = nonBlocking;
}

void ASocket::setTimeout(uint32_t milliseconds)
{
    _timeout = milliseconds;
}

std::string ASocket::getLastError() const
{
    return _lastError;
}

void ASocket::setError(const std::string& error)
{
    _lastError = error;
}

bool ASocket::validatePacket(const protocol::Packet& packet) const
{
    // Basic packet validation
    if (packet.data.size() > 65507) { // Max UDP packet size - headers
        return false;
    }
    
    // Could add more validation here
    return true;
}

void ASocket::setConnectionState(bool connected)
{
    _connected = connected;
}

} // namespace network
} // namespace common
