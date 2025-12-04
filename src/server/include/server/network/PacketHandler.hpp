#pragma once

#include "../../common/include/common/protocol/Packet.hpp"
#include <functional>
#include <unordered_map>

namespace server {
namespace network {

class PacketHandler {
public:
    using HandlerFunction = std::function<void(uint32_t clientId, const common::protocol::Packet&)>;

    void registerHandler(uint8_t packetType, HandlerFunction handler);
    void handlePacket(uint32_t clientId, const common::protocol::Packet& packet);

private:
    std::unordered_map<uint8_t, HandlerFunction> m_handlers;
};

} // namespace network
} // namespace server
