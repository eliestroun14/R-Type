#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include "../../../../common/include/common/protocol/Protocol.hpp"
#include "../network/ServerNetworkManager.hpp"
#include <atomic>
#include <deque>
#include "../../../common/include/common/protocol/Packet.hpp"
#include "../../../common/include/common/constants/defines.hpp"
#include "src/engine/include/engine/gameEngine/GameEngine.hpp"


namespace server {

    /**
     * @brief Configuration du serveur
     */
    struct ServerConfig {
        uint16_t port = 4242;
        uint32_t maxPlayers = 16;
        uint32_t tickRate = 60;  // Hz
        std::chrono::milliseconds heartbeatTimeout = std::chrono::milliseconds(TIMEOUT);
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

        void networkLoop();
        
        void gameLoop();

        std::deque<common::protocol::Packet>& getPacketsReceived() { return _packetsReceived; }
        std::deque<common::protocol::Packet>& getPacketsToSend() { return _packetsToSend; }

        //std::shared_ptr<engine::GameEngine> getGameEngine() const { return _gameEngine; }    // TODO

    private:
        // Configuration
        ServerConfig _config;

        std::unique_ptr<server::network::ServerNetworkManager> _networkManager;

        // State
        std::atomic<bool> _isRunning;

        //std::shared_ptr<engine::GameEngine> _gameEngine;

        std::deque<common::protocol::Packet> _packetsReceived;
        std::deque<common::protocol::Packet> _packetsToSend;

    };

} // namespace server

#endif // SERVER_HPP_
