/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RTypeClient
*/

#include "RTypeClient.hpp"

RTypeClient::RTypeClient()
{
}

RTypeClient::~RTypeClient()
{
}

void RTypeClient::init(const char *serverIp, uint16_t port)
{
}

bool RTypeClient::connect(const char *serverIp, uint16_t port)
{
    return false;
}

void RTypeClient::sendInput(uint16_t inputFlags)
{
}

void RTypeClient::sendHeartbeat()
{
    protocol::PacketHeader *header;

    header->packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEARTBEAT);
    send(this->_sockfd, header, sizeof(protocol::PacketHeader), NULL);
}

void RTypeClient::processWorldSnapshot()
{
}

void RTypeClient::run()
{
    sf::Time time = this->_clock.getElapsedTime();

    while (true) {

        sendHeartbeat();
    }
}
