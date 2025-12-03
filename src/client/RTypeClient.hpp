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
#include "src/server/protocol/protocol.hpp"

#define TIMEOUT 15000

class RTypeClient {
    public:
        RTypeClient();
        ~RTypeClient();

        void init(const char* serverIp, uint16_t port);

        bool waitForAccept(protocol::ServerAccept *serverAccept);
        bool connect(const char* serverIp, uint16_t port, std::string playerName);

        void sendInput(uint16_t inputFlags);
        uint16_t getInput() const;
        void sendHeartbeat();

        void processWorldSnapshot();

        void run();

    private:
        int _sockfd;
        struct sockaddr_in _serverAddr;
        uint32_t _sequenceNumber;
        uint32_t _playerId;
        bool _connected;

        sf::Clock _clock;
};

#endif /* !RTYPECLIENT_HPP_ */
