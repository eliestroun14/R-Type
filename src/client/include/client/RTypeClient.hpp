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

        //void networkLoop();

        void gameLoop();

        // getters and setters
        void setSelfId(uint32_t id) { _selfId = id; }
        uint32_t getSelfId() const { return _selfId; }
        void setPlayerName(const std::string& name) { _playerName = name; }
        std::string getPlayerName() const { return _playerName; }
        bool isConnected() const { return _isConnected; }
        void setConnected(bool status) { _isConnected = status; }


    private:
        uint32_t calculateLatency(uint32_t ping_sent_time, uint32_t ping_received_time);
        std::atomic<bool>& isRunning() { return _isRunning; }

        std::shared_ptr<gameEngine::GameEngine> getGameEngine() const { return _gameEngine; }    // TODO

    private:
        std::unique_ptr<client::network::ClientNetworkManager> _networkManager;
        uint32_t _selfId;                                                       // assigned by server at connection (probably the socket id)
        std::string _playerName;
        std::atomic<bool> _isRunning;
        std::atomic<unsigned int> tickCount;
        std::atomic<bool> _isConnected;

        std::shared_ptr<gameEngine::GameEngine> _gameEngine;                        // game engine that will do a lot of things for us TODO

};

#endif /* !RTYPECLIENT_HPP_ */
