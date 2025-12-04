#pragma once

#include <cstdint>
#include <string>

namespace common {
namespace network {

using ClientId = uint32_t;
using SessionId = uint32_t;
using PacketId = uint32_t;

enum class ConnectionState : uint8_t {
    DISCONNECTED = 0,
    CONNECTING = 1,
    CONNECTED = 2,
    DISCONNECTING = 3
};

struct NetworkStats {
    uint64_t bytesSent = 0;
    uint64_t bytesReceived = 0;
    uint32_t packetsSent = 0;
    uint32_t packetsReceived = 0;
    uint32_t packetsLost = 0;
    float averageLatency = 0.0f;
};

} // namespace network
} // namespace common
