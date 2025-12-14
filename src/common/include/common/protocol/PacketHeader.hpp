#pragma once

#include <cstdint>

namespace common {
namespace protocol {

struct PacketHeader {
    uint8_t type;
    uint16_t length;
    uint32_t sequence;
    uint32_t timestamp;

    static constexpr size_t SIZE = sizeof(type) + sizeof(length) + sizeof(sequence) + sizeof(timestamp);
};

} // namespace protocol
} // namespace common
