#pragma once

#include "../session/PlayerSession.hpp"
#include <memory>
#include <vector>

namespace server {
namespace network {

class ClientConnection {
public:
    ClientConnection(uint32_t clientId);
    ~ClientConnection();

    uint32_t getClientId() const { return m_clientId; }
    bool isConnected() const { return m_connected; }

    void send(const std::vector<uint8_t>& data);
    void disconnect();

    float getLatency() const { return m_latency; }
    void updateLatency(float latency) { m_latency = latency; }

private:
    uint32_t m_clientId;
    bool m_connected;
    float m_latency;
    uint32_t m_lastHeartbeat;
};

} // namespace network
} // namespace server
