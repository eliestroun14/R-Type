#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace client {
namespace network {

class ServerConnection {
public:
    ServerConnection();
    ~ServerConnection();

    bool connect(const std::string& address, uint16_t port);
    void disconnect();
    bool isConnected() const { return m_connected; }

    void send(const std::vector<uint8_t>& data);
    bool receive(std::vector<uint8_t>& data);

    float getLatency() const { return m_latency; }

private:
    bool m_connected;
    float m_latency;
    std::string m_serverAddress;
    uint16_t m_serverPort;
};

} // namespace network
} // namespace client
