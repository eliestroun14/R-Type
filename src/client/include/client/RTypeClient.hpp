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
#include "../../../common/include/common/AsioSocket.hpp"
#include "../../../engine/include/engine/GameEngine.hpp"
#include <atomic>
#include <deque>
#include "../../../common/include/common/protocol/Packet.hpp"


#define TIMEOUT 15000
#define HEARTBEAT_INTERVAL 5000
#define TICK_RATE 16

#define HEARTBEAT_TICK_INTERVAL 300
#define INPUT_SEND_TICK_INTERVAL 2


class RTypeClient {
    public:
        RTypeClient();
        ~RTypeClient();

        void init(const char* serverIp, uint16_t port, std::string playerName);


        void run();


        void stop();

        void networkLoop();
        
        void gameLoop();

    private:
        std::shared_ptr<common::network::AsioSocket> _server;                   // relation with server , need to create a AsioSocket
        uint32_t _selfId;
        std::atomic<bool> _isRunning;

        std::shared_ptr<engine::GameEngine> _gameEngine;                        // game engine that will do a lot of things for us

        std::deque<common::protocol::Packet> _packetsReceived;
        std::deque<common::protocol::Packet> _packetsToSend;

};

#endif /* !RTYPECLIENT_HPP_ */
