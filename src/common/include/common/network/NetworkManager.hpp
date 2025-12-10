/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** INetworkManager
*/

#ifndef INETWORKMANAGER_HPP_
#define INETWORKMANAGER_HPP_

#include <atomic>
#include <optional>
#include <vector>
#include "protocol/Packet.hpp"

namespace common {
namespace network {

struct ReceivedPacket {
    common::protocol::Packet packet;
    std::optional<uint32_t> clientId; // unset on client side
};

class INetworkManager {
public:
    virtual ~INetworkManager() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;

    virtual void queueOutgoing(const common::protocol::Packet& packet,
                               std::optional<uint32_t> targetClient = std::nullopt) = 0;

    virtual std::vector<ReceivedPacket> fetchIncoming() = 0;
};

} // namespace network
} // namespace common

#endif /* !INETWORKMANAGER_HPP_ */
