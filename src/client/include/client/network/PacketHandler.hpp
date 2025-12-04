#pragma once

#include "../../common/include/common/protocol/Packet.hpp"
#include <functional>
#include <unordered_map>

namespace client {
namespace network {

class PacketHandler {
public:
    using HandlerFunction = std::function<void(const common::protocol::Packet&)>;

    void registerHandler(uint8_t packetType, HandlerFunction handler);
    void handlePacket(const common::protocol::Packet& packet);

private:
    std::unordered_map<uint8_t, HandlerFunction> m_handlers;
};

} // namespace network
} // namespace client
