/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RTypeClient
*/

#ifndef RTYPECLIENT_HPP_
#define RTYPECLIENT_HPP_

// Platform-specific includes for network compatibility
#ifdef _WIN32
    #define NOMINMAX
    #define _WIN32_WINNT 0x0601  // Windows 7 or later
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    // Linux/Unix includes
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include <cstdint>
#include <SFML/Graphics.hpp>
#include <memory>
#include <common/protocol/Protocol.hpp>
#include <client/network/ClientNetworkManager.hpp>
#include <engine/GameEngine.hpp>
#include <game/Game.hpp>
#include <atomic>
#include <deque>
#include <common/protocol/Packet.hpp>
#include <common/constants/defines.hpp>



class RTypeClient {
    public:
        RTypeClient();
        ~RTypeClient();
        
        void init(const char* serverIp, uint16_t port, std::string playerName);


        void run();


        void stop();

        // Test and monitoring helpers
        bool isRunningFlag() const { return _isRunning.load(); }
        bool isConnectedFlag() const { return _isConnected.load(); }
        client::network::ClientNetworkManager* getNetworkManager() const { return _networkManager.get(); }
        Game* getGame() const { return _game.get(); }
        void setNetworkManager(std::unique_ptr<client::network::ClientNetworkManager> manager) { _networkManager = std::move(manager); }
        void resetGame() { _game.reset(); }
        void setRunning(bool running) { _isRunning = running; }

        // getters and setters
        void setPlayerName(const std::string& name) { _playerName = name; }
        std::string getPlayerName() const { return _playerName; }
        bool isConnected() const { return _isConnected; }
        void setConnected(bool status) { _isConnected = status; if (_game) _game->setConnected(status); }

    private:
        std::atomic<bool>& isRunning() { return _isRunning; }

    private:
        // Game instance
        std::unique_ptr<Game> _game;
        std::unique_ptr<client::network::ClientNetworkManager> _networkManager;
        std::string _playerName;
        std::atomic<bool> _isRunning;
        std::atomic<bool> _isConnected;
};

#endif /* !RTYPECLIENT_HPP_ */
