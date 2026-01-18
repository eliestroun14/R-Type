#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>

namespace common {
namespace protocol {

struct PacketHeader {
    uint16_t    magic = 0x5254;                     // Protocol identifier 'RT' for R-Type
    uint8_t     packet_type = 0;                    // Packet type identifier
    uint8_t     flags = 0;                          // Control flags
    uint32_t    sequence_number = 0;                // Monotonic sequence number
    uint32_t    timestamp = 0;                      // Milisec since connection
};

class Packet {
public:
    PacketHeader header;
    std::vector<uint8_t> data;

    Packet() = default;
    
    explicit Packet(uint8_t type) {
        header.packet_type = type;
        header.magic = 0x5254;
        header.flags = 0;
        header.sequence_number = 0;
        header.timestamp = 0;
    }

    explicit Packet(uint8_t type, uint8_t flags, uint32_t sequence_number, uint32_t timestamp) {
        header.packet_type = type;
        header.flags = flags;
        header.sequence_number = sequence_number;
        header.timestamp = timestamp;
        header.magic = 0x5254;
    }

    explicit Packet(const PacketHeader& header, const std::vector<uint8_t>& data)
        : header(header), data(data) {}

    void setData(const std::string& data) {
        this->data.assign(data.begin(), data.end());
    }
    
    std::string getData() const {
        return std::string(data.begin(), data.end());
    }

    void serialize(std::vector<uint8_t>& buffer) const {
        // Stub implementation - just copy header and data
        buffer.clear();
        buffer.resize(sizeof(PacketHeader) + data.size());
        std::memcpy(buffer.data(), &header, sizeof(PacketHeader));
        if (!data.empty()) {
            std::memcpy(buffer.data() + sizeof(PacketHeader), data.data(), data.size());
        }
    }
    
    bool deserialize(const std::vector<uint8_t>& buffer) {
        // Stub implementation
        if (buffer.size() < sizeof(PacketHeader)) {
            return false;
        }
        std::memcpy(&header, buffer.data(), sizeof(PacketHeader));
        if (buffer.size() > sizeof(PacketHeader)) {
            data.assign(buffer.begin() + sizeof(PacketHeader), buffer.end());
        }
        return true;
    }
};

} // namespace protocol
} // namespace common
