#pragma once

#include <cstdint>
#include <vector>

namespace common {
namespace protocol {

struct PacketHeader {
    uint8_t type;
    uint16_t length;
    uint32_t sequence;
    uint32_t timestamp;
};

class Packet {
public:
    PacketHeader header;
    std::vector<uint8_t> data;

    Packet() = default;
    explicit Packet(uint8_t type);

    void serialize(std::vector<uint8_t>& buffer) const;
    bool deserialize(const std::vector<uint8_t>& buffer);
};

} // namespace protocol
} // namespace common
