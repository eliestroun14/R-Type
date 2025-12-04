#pragma once

#include <memory>
#include <vector>

namespace server {
namespace core {

class GameInstance;
class NetworkManager;

class Server {
public:
    Server();
    ~Server();

    bool initialize(uint16_t port);
    void run();
    void shutdown();

    bool isRunning() const { return m_running; }

private:
    void update(float deltaTime);
    void processNetworkEvents();

    bool m_running;
    std::unique_ptr<NetworkManager> m_networkManager;
    std::vector<std::unique_ptr<GameInstance>> m_gameInstances;
};

} // namespace core
} // namespace server
