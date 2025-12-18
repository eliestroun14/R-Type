#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <common/protocol/Protocol.hpp>
#include <common/network/sockets/AsioSocket.hpp>
#include <atomic>
#include <deque>
#include <common/protocol/Packet.hpp>
#include <common/constants/defines.hpp>
#include <engine/GameEngine.hpp>
#include <server/network/ServerNetworkManager.hpp>


namespace server {

    /**
     * @brief Configuration du serveur
     */
    struct ServerConfig {
        uint16_t port = 4242;
        uint32_t maxPlayers = 2;
        uint32_t tickRate = 60;  // Hz
        std::chrono::milliseconds heartbeatTimeout = std::chrono::milliseconds(TIMEOUT_MS);
        bool enableLogging = true;
    };

    /**
     * @brief Serveur principal R-Type
     * Orchestre les différentes couches (Network, Protocol, Session, Game Logic)
     */
    class Server {
    public:
        explicit Server(const ServerConfig& config = ServerConfig{});
        ~Server();

        // Empêcher la copie
        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;


        bool init();

        void run();

        void stop();

        bool isRunning() const { return _isRunning.load(); }

        void printStatistics() const;

        void gameLoop();

    private:
        // Configuration
        ServerConfig _config;

        std::unique_ptr<server::network::ServerNetworkManager> _networkManager;

        // State
        std::atomic<bool> _isRunning;

        std::shared_ptr<gameEngine::GameEngine> _gameEngine;

    };

} // namespace server

#endif // SERVER_HPP_
