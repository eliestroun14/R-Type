#pragma once

#include <memory>
#include <cstdint>

namespace client {
namespace core {

class GameState;
class NetworkManager;

class Client {
public:
    Client();
    ~Client();

    bool initialize();
    void run();
    void shutdown();

    bool connect(const std::string& serverAddress, uint16_t port);
    void disconnect();

    bool isRunning() const { return m_running; }

private:
    void update(float deltaTime);
    void render();
    void processInput();

    bool m_running;
    std::unique_ptr<GameState> m_gameState;
    std::unique_ptr<NetworkManager> m_networkManager;
};

} // namespace core
} // namespace client
