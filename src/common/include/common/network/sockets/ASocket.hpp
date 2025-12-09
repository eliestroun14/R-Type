/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ASocket
*/

#ifndef ASOCKET_HPP_
#define ASOCKET_HPP_

#include <cstdint>
#include <string>
#include <atomic>
#include <chrono>
#include "ISocket.hpp"
#include "../../protocol/Packet.hpp"

namespace common {
namespace network {

/**
 * @brief Abstract base class providing common socket functionality
 * 
 * This class implements the ISocket interface and provides common functionality
 * that can be shared across different socket implementations. It handles state
 * management, validation, and provides helper methods.
 */
class ASocket : public ISocket {
public:
    ASocket();
    virtual ~ASocket();

    // Implemented ISocket methods with common logic
    bool isConnected() const override;
    uint16_t getLocalPort() const override;
    std::string getRemoteAddress() const override;
    uint16_t getRemotePort() const override;

    /**
     * @brief Set socket to non-blocking mode
     * @param nonBlocking true for non-blocking, false for blocking
     */
    virtual void setNonBlocking(bool nonBlocking);

    /**
     * @brief Set timeout for socket operations
     * @param milliseconds Timeout in milliseconds
     */
    virtual void setTimeout(uint32_t milliseconds);

    /**
     * @brief Get the last error message
     * @return The last error message as a string
     */
    std::string getLastError() const;

protected:
    // Protected members accessible by derived classes
    std::atomic<bool> _connected;
    std::atomic<bool> _nonBlocking;
    uint16_t _localPort;
    uint16_t _remotePort;
    std::string _remoteAddress;
    std::string _lastError;
    uint32_t _timeout;

    /**
     * @brief Set the last error message
     * @param error The error message to set
     */
    void setError(const std::string& error);

    /**
     * @brief Validate packet before sending
     * @param packet The packet to validate
     * @return true if valid, false otherwise
     */
    bool validatePacket(const protocol::Packet& packet) const;

    /**
     * @brief Update connection state
     * @param connected The new connection state
     */
    void setConnectionState(bool connected);
};

} // namespace network
} // namespace common

#endif /* !ASOCKET_HPP_ */
