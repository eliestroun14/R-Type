#pragma once

#include <cstdint>
#include <vector>

namespace common {
namespace protocol {

struct PacketHeader {
    uint16_t    magic = 0x5254;                     // Protocol identifier 'RT' for R-Type
    uint8_t     packet_type;                        // Packet type identifier
    uint8_t     flags;                              // Control flags
    uint32_t    sequence_number;                    // Monotonic sequence nuumber
    uint32_t    timestamp;                          // Milisec since connection
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
