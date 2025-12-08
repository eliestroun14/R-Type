#pragma once

#include <queue>
#include <unordered_map>
#include <cstdint>

namespace server {
namespace network {

struct PendingPacket {
    uint32_t sequence;
    std::vector<uint8_t> data;
    uint32_t timestamp;
    uint8_t retryCount;
};

class ReliabilityManager {
public:
    void sendReliable(uint32_t clientId, const std::vector<uint8_t>& data);
    void handleAck(uint32_t clientId, uint32_t sequence);
    void update(uint32_t currentTime);

private:
    void retransmit(uint32_t clientId, const PendingPacket& packet);

    std::unordered_map<uint32_t, std::queue<PendingPacket>> m_pendingPackets;
    uint32_t m_nextSequence = 0;
};

} // namespace network
} // namespace server
