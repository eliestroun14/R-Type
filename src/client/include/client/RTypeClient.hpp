/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RTypeClient
*/

#ifndef RTYPECLIENT_HPP_
#define RTYPECLIENT_HPP_

#include <cstdint>
#include <sys/socket.h>
#include <SFML/Graphics.hpp>
#include <netinet/in.h>
#include <common/protocol/Protocol.hpp>
#include <client/network/ClientNetworkManager.hpp>
#include <engine/GameEngine.hpp>
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

        // getters and setters
        void setPlayerName(const std::string& name) { _playerName = name; }
        std::string getPlayerName() const { return _playerName; }
        bool isConnected() const { return _isConnected; }
        void setConnected(bool status) { _isConnected = status; }

    private:
        std::atomic<bool>& isRunning() { return _isRunning; }

    private:
        // TODO: add game instance
        std::unique_ptr<client::network::ClientNetworkManager> _networkManager;
        std::string _playerName;
        std::atomic<bool> _isRunning;
        std::atomic<bool> _isConnected;
};

#endif /* !RTYPECLIENT_HPP_ */
