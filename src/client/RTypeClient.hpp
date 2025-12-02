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
#include "src/server/protocol/protocol.hpp"

class RTypeClient {
    public:
        RTypeClient();
        ~RTypeClient();

        void init(const char* serverIp, uint16_t port);

        bool connect(const char* serverIp, uint16_t port);

        void sendInput(uint16_t inputFlags);
        void sendHeartbeat();

        void processWorldSnapshot();

        void run();

    private:
        int _sockfd;
        struct sockaddr_in *_serverAddr;
        uint32_t _sequenceNumber;
        uint32_t _playerId;
        bool _connected;
        sf::Clock _clock;

        uint32_t _playerId;
};

#endif /* !RTYPECLIENT_HPP_ */
