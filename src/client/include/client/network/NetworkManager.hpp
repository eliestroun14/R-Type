#pragma once

#include <string>
#include <cstdint>
#include <functional>

namespace client {
namespace network {

class ServerConnection;
class PacketHandler;

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    bool connect(const std::string& address, uint16_t port);
    void disconnect();
    bool isConnected() const { return m_connected; }

    void send(const std::vector<uint8_t>& data);
    void update();

    void setOnDisconnect(std::function<void()> callback);

private:
    bool m_connected;
    std::unique_ptr<ServerConnection> m_connection;
    std::unique_ptr<PacketHandler> m_packetHandler;
};

} // namespace network
} // namespace client
