/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ISocket
*/

#ifndef ISOCKET_HPP_
#define ISOCKET_HPP_

#include <cstdint>
#include <string>
#include <common/protocol/Packet.hpp>

namespace common {
namespace network {

/**
 * @brief Pure virtual interface for socket operations
 * 
 * This interface defines the contract that all socket implementations must follow.
 * It provides methods for connection management, data transmission, and socket configuration.
 */
class ISocket {
public:
    virtual ~ISocket() = default;

    /**
     * @brief Connect to a remote endpoint
     * @param host The hostname or IP address to connect to
     * @param port The port number to connect to
     * @return true if connection successful, false otherwise
     */
    virtual bool connect(const std::string& host, uint16_t port) = 0;

    /**
     * @brief Bind the socket to a specific port (for servers)
     * @param port The port number to bind to
     * @return true if bind successful, false otherwise
     */
    virtual bool bind(uint16_t port) = 0;

    /**
     * @brief Send a packet through the socket
     * @param packet The packet to send
     * @return true if send successful, false otherwise
     */
    virtual bool send(const protocol::Packet& packet) = 0;

    /**
     * @brief Receive a packet from the socket (non-blocking poll)
     * @param packet Reference to store the received packet
     * @return true if a packet was received, false otherwise
     */
    virtual bool receive(protocol::Packet& packet) = 0;

    /**
     * @brief Close the socket connection
     */
    virtual void close() = 0;

    /**
     * @brief Check if the socket is connected
     * @return true if connected, false otherwise
     */
    virtual bool isConnected() const = 0;

    /**
     * @brief Check if there's data available to read (polling)
     * @return true if data is available, false otherwise
     */
    virtual bool hasData() const = 0;

    /**
     * @brief Get the local port number
     * @return The local port number, or 0 if not bound
     */
    virtual uint16_t getLocalPort() const = 0;

    /**
     * @brief Get the remote endpoint address
     * @return The remote address as a string, or empty if not connected
     */
    virtual std::string getRemoteAddress() const = 0;

    /**
     * @brief Get the remote endpoint port
     * @return The remote port number, or 0 if not connected
     */
    virtual uint16_t getRemotePort() const = 0;
};

} // namespace network
} // namespace common

#endif /* !ISOCKET_HPP_ */
