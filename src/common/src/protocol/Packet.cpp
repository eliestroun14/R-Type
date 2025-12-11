/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Packet implementation
*/

#include <common/protocol/Packet.hpp>
#include <cstring>

namespace common {
namespace protocol {

Packet::Packet(uint8_t type)
{
    header.magic = 0x5254;           // 'RT' magic number
    header.packet_type = type;       // Set packet type
    header.flags = 0;                // Initialize flags to 0
    header.sequence_number = 0;      // Initialize sequence to 0
    header.timestamp = 0;            // Initialize timestamp to 0
    data.clear();                    // Empty payload
}

void Packet::serialize(std::vector<uint8_t>& buffer) const
{
    buffer.clear();

    // Serialize header fields
    const uint8_t* magic_ptr = reinterpret_cast<const uint8_t*>(&header.magic);
    buffer.insert(buffer.end(), magic_ptr, magic_ptr + sizeof(header.magic));

    buffer.push_back(header.packet_type);
    buffer.push_back(header.flags);

    const uint8_t* seq_ptr = reinterpret_cast<const uint8_t*>(&header.sequence_number);
    buffer.insert(buffer.end(), seq_ptr, seq_ptr + sizeof(header.sequence_number));

    const uint8_t* ts_ptr = reinterpret_cast<const uint8_t*>(&header.timestamp);
    buffer.insert(buffer.end(), ts_ptr, ts_ptr + sizeof(header.timestamp));

    // Serialize payload data
    buffer.insert(buffer.end(), data.begin(), data.end());
}

bool Packet::deserialize(const std::vector<uint8_t>& buffer)
{
    // Minimum size check: header must be at least 12 bytes
    // (2 bytes magic + 1 byte type + 1 byte flags + 4 bytes seq + 4 bytes timestamp)
    constexpr size_t HEADER_SIZE = sizeof(PacketHeader);
    if (buffer.size() < HEADER_SIZE) {
        return false;
    }

    size_t offset = 0;

    // Deserialize magic number
    std::memcpy(&header.magic, buffer.data() + offset, sizeof(header.magic));
    offset += sizeof(header.magic);

    // Verify magic number
    if (header.magic != 0x5254) {
        return false;
    }

    // Deserialize packet type
    header.packet_type = buffer[offset++];

    // Deserialize flags
    header.flags = buffer[offset++];

    // Deserialize sequence number
    std::memcpy(&header.sequence_number, buffer.data() + offset, sizeof(header.sequence_number));
    offset += sizeof(header.sequence_number);

    // Deserialize timestamp
    std::memcpy(&header.timestamp, buffer.data() + offset, sizeof(header.timestamp));
    offset += sizeof(header.timestamp);

    // Deserialize payload data
    data.clear();
    if (offset < buffer.size()) {
        data.insert(data.end(), buffer.begin() + offset, buffer.end());
    }

    return true;
}

} // namespace protocol
} // namespace common
