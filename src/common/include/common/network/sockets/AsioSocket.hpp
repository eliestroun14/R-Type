/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** AsioSocket
*/

#ifndef ASIOSOCKET_HPP_
#define ASIOSOCKET_HPP_

#include <memory>
#include <asio.hpp>
#include <common/network/sockets/ASocket.hpp>
#include <common/protocol/Packet.hpp>

namespace common {
namespace network {

/**
 * @brief Concrete socket implementation using ASIO library
 * 
 * This class provides a full UDP socket implementation using the ASIO library.
 * It handles asynchronous I/O operations, connection management, and provides
 * efficient non-blocking network operations suitable for real-time game networking.
 */
class AsioSocket : public ASocket {
public:
    /**
     * @brief Default constructor for server-side sockets
     */
    AsioSocket();

    /**
     * @brief Constructor for client-side sockets with immediate connection
     * @param host The hostname or IP address to connect to
     * @param port The port number to connect to
     */
    AsioSocket(const std::string& host, uint16_t port);

    /**
     * @brief Destructor - ensures proper cleanup
     */
    ~AsioSocket() override;

    // ISocket interface implementation
    bool connect(const std::string& host, uint16_t port) override;
    bool bind(uint16_t port) override;
    bool send(const protocol::Packet& packet) override;
    bool receive(protocol::Packet& packet) override;
    void close() override;
    bool hasData() const override;

    /**
     * @brief Receive packet from any sender and get sender address
     * @param packet The packet to receive into
     * @param remoteAddress Output: sender address as "ip:port"
     * @return true if packet received, false otherwise
     */
    bool receiveFrom(protocol::Packet& packet, std::string& remoteAddress);

    /**
     * @brief Send packet to specific address
     * @param packet The packet to send
     * @param remoteAddress Target address as "ip:port"
     * @return true if packet sent, false otherwise
     */
    bool sendTo(const protocol::Packet& packet, const std::string& remoteAddress);

    /**
     * @brief Set socket buffer sizes
     * @param sendBufferSize Send buffer size in bytes
     * @param receiveBufferSize Receive buffer size in bytes
     */
    void setBufferSizes(size_t sendBufferSize, size_t receiveBufferSize);

    /**
     * @brief Enable or disable broadcast mode
     * @param enable true to enable broadcast, false to disable
     */
    void setBroadcast(bool enable);

    /**
     * @brief Set the maximum packet size
     * @param maxSize Maximum packet size in bytes
     */
    void setMaxPacketSize(size_t maxSize);

private:
    std::unique_ptr<asio::io_context> _ioContext;
    std::unique_ptr<asio::ip::udp::socket> _socket;
    asio::ip::udp::endpoint _remoteEndpoint;
    asio::ip::udp::endpoint _senderEndpoint;
    
    size_t _maxPacketSize;
    std::vector<uint8_t> _receiveBuffer;

    /**
     * @brief Initialize the socket with default settings
     */
    void initializeSocket();

    /**
     * @brief Resolve hostname to IP address
     * @param host The hostname to resolve
     * @param port The port number
     * @return The resolved endpoint, or empty endpoint on failure
     */
    asio::ip::udp::endpoint resolveEndpoint(const std::string& host, uint16_t port);
};

} // namespace network
} // namespace common

#endif /* !ASIOSOCKET_HPP_ */
