/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RTypeClient
*/

#include "RTypeClient.hpp"
#include <string.h>
#include <arpa/inet.h>
#include <src/utils/client/ClientUtils.hpp>
#include <thread>

RTypeClient::RTypeClient() : _sockfd(-1), _sequenceNumber(0), _playerId(0), _connected(false)
{
}

RTypeClient::~RTypeClient()
{
}

void RTypeClient::init(const char *serverIp, uint16_t port, std::string playerName)
{
    if (connect(serverIp, port, playerName) == false)
        throw std::runtime_error("Error connecting to server");
}

bool RTypeClient::waitForAccept(protocol::ServerAccept *serverAccept)
{
    uint32_t startTime = getCurrentTimeMs();
    const uint32_t timeout = TIMEOUT;

    while (true)
    {
        if (serverAccept->header.packet_type ==
            static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_ACCEPT))
            return true;

        uint32_t currentTime = getCurrentTimeMs();
        if (currentTime - startTime >= timeout)
            return false;

        // mini pause pour pas full cosommer le CPU, à voir si le garde, si ça met trop de latence on enlève
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return false;
}

bool RTypeClient::connect(const char *serverIp, uint16_t port, std::string playerName)
{
    // Create UDP socket
    this->_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->_sockfd)
        return false;

    // Setup server address
    memset(&this->_serverAddr, 0, sizeof(this->_serverAddr));
    this->_serverAddr.sin_family = AF_INET;
    this->_serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIp, &this->_serverAddr.sin_addr);

    // Send CLIENT_CONNECT
    protocol::ClientConnect connectPacket;
    connectPacket.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_CONNECT);
    connectPacket.header.flags = 0;
    connectPacket.header.sequence_number = this->_sequenceNumber++;
    connectPacket.header.timestamp = getCurrentTimeMs();
    connectPacket.protocol_version = 1;
    strncpy(connectPacket.player_name, playerName.c_str(), 31);
    connectPacket.client_id = generateClientId();

    //Serialize and send
    uint8_t buffer[256];
    // serializeClientConnect(&connectPacket, buffer); TODO: faire serializeClientConnect()
    sendto(this->_sockfd, buffer, sizeof(protocol::ClientConnect), 0,
        (struct sockaddr*)&this->_serverAddr, sizeof(this->_serverAddr));


    // Wait for SERVER_ACCEPT
    protocol::ServerAccept serverAccept;

    if (waitForAccept(&serverAccept)) {
        this->_connected = true;
        this->_playerId = serverAccept.assigned_player_id;
        return true;
    }

    return false;
}

void RTypeClient::sendInput(uint16_t inputFlags)
{
}

void RTypeClient::sendHeartbeat()
{
    protocol::HeartBeat heartbeat;

    heartbeat.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEARTBEAT);
    heartbeat.player_id = this->_playerId;
    send(this->_sockfd, &heartbeat, sizeof(protocol::HeartBeat), NULL);
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
