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
#include "../../../common/include/common/protocol/Protocol.hpp"
#include "network/ClientNetworkManager.hpp"
//#include "../../../engine/include/engine/GameEngine.hpp"                      // TODO
#include <atomic>
#include <deque>
#include "../../../common/include/common/protocol/Packet.hpp"
#include "../../../common/include/common/constants/defines.hpp"




class RTypeClient {
    public:
        RTypeClient();
        ~RTypeClient();

        void init(const char* serverIp, uint16_t port, std::string playerName);


        void run();


        void stop();

        void networkLoop();
        
        void gameLoop();

        std::string getPlayerName() const { return _playerName; }
        uint32_t getSelfId() const { return _selfId; }
        std::atomic<bool>& isRunning() { return _isRunning; }
        std::deque<common::protocol::Packet>& getPacketsReceived() { return _packetsReceived; }
        std::deque<common::protocol::Packet>& getPacketsToSend() { return _packetsToSend; }

        //std::shared_ptr<engine::GameEngine> getGameEngine() const { return _gameEngine; }    // TODO

    private:
        std::unique_ptr<client::network::ClientNetworkManager> _networkManager;
        uint32_t _selfId;                                                       // assigned by server at connection (probably the socket id)
        std::string _playerName;
        std::atomic<bool> _isRunning;

        //std::shared_ptr<engine::GameEngine> _gameEngine;                        // game engine that will do a lot of things for us TODO

        std::deque<common::protocol::Packet> _packetsReceived;
        std::deque<common::protocol::Packet> _packetsToSend;

};

#endif /* !RTYPECLIENT_HPP_ */
