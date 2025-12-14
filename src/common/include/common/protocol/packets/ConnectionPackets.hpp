#pragma once

#include <common/protocol/Packet.hpp>
#include <string>

namespace common {
namespace protocol {
namespace packets {

struct ConnectRequest {
    std::string clientVersion;
    std::string playerName;
};

struct ConnectResponse {
    bool accepted;
    uint32_t playerId;
    std::string reason;
};

struct Disconnect {
    uint32_t playerId;
    std::string reason;
};

struct Heartbeat {
    uint32_t clientTime;
};

struct HeartbeatAck {
    uint32_t clientTime;
    uint32_t serverTime;
};

} // namespace packets
} // namespace protocol
} // namespace common
