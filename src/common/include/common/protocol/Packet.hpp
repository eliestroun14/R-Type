#pragma once

#include <cstdint>
#include <vector>
#include <string>

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

    explicit Packet(uint8_t type, uint8_t flags, uint32_t sequence_number, uint32_t timestamp);

    explicit Packet(const PacketHeader& header, const std::vector<uint8_t>& data)
        : header(header), data(data) {}

    void setData(const std::string& data) {
        this->data.assign(data.begin(), data.end());
    }
    std::string getData() const {
        return std::string(data.begin(), data.end());
    }

    void serialize(std::vector<uint8_t>& buffer) const;
    bool deserialize(const std::vector<uint8_t>& buffer);
};

} // namespace protocol
} // namespace common
