#pragma once

#include <common/protocol/Packet.hpp>

namespace common {
namespace protocol {
namespace packets {

struct Ack {
    uint32_t sequence;
    uint32_t timestamp;
};

struct Nack {
    uint32_t sequence;
    uint8_t reason;
};

struct Ping {
    uint32_t timestamp;
    uint32_t sequence;
};

struct Pong {
    uint32_t timestamp;
    uint32_t sequence;
};

} // namespace packets
} // namespace protocol
} // namespace common
