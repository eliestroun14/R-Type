/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Packet implementation
*/

#include "../../include/common/protocol/Packet.hpp"
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
    // TODO: Implement serialization
}

bool Packet::deserialize(const std::vector<uint8_t>& buffer)
{
    // TODO: Implement deserialization
    return false;
}

} // namespace protocol
} // namespace common
