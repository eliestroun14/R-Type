/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RTypeClient
*/

#include "../include/client/RTypeClient.hpp"
#include <string.h>
#include <arpa/inet.h>
#include "../include/client/utils/ClientUtils.hpp"
#include <thread>
#include <chrono>
#include <iostream>





RTypeClient::RTypeClient() : _selfId(0), _isRunning(false)
{
}

RTypeClient::~RTypeClient()
{
}

void RTypeClient::init(const char* serverIp, uint16_t port, std::string playerName)
{
    this->_server = std::make_shared<common::network::AsioSocket>(serverIp, port);
    //this->_gameEngine = std::make_shared<engine::GameEngine>();             // TODO

    this->_packetsReceived.clear();
    this->_packetsToSend.clear();

    this->_playerName = playerName;
    this->_isRunning = false;
    // Additional initialization code can be added here
}

void RTypeClient::run()
{
    _isRunning = true;

    std::thread networkThread(&RTypeClient::networkLoop, this);

    std::thread gameThread(&RTypeClient::gameLoop, this);

    networkThread.join();
    gameThread.join();
}

void RTypeClient::stop()
{
    _isRunning = false;
}

void RTypeClient::networkLoop()
{
    while (_isRunning) {
        common::protocol::Packet rawPacket;
        if (this->_server->receive(rawPacket)) {                                // polling receive from server handled by the AsioSocket class TODO
            this->_packetsReceived.push_back(rawPacket);
        }

        
        common::protocol::Packet out;
        while (!this->_packetsToSend.empty()) {
            out = this->_packetsToSend.front();
            this->_server->send(out);
            this->_packetsToSend.pop_front();
        }
    }
}

void RTypeClient::gameLoop()
{
    auto next = std::chrono::steady_clock::now();
    const std::chrono::milliseconds tick(std::chrono::milliseconds(TICK_RATE));

    uint64_t currentTick = 0;
    uint64_t lastHeartbeatTick = 0;
    uint64_t lastInputSendTick = 0;

    while (_isRunning) {
        next += tick;
        std::this_thread::sleep_until(next);

        currentTick++;

        // Calculate delta time in milliseconds
        float deltaTime = static_cast<float>(TICK_RATE);

        // Process received packets
        std::vector<common::protocol::Packet> packetsToProcess;
        size_t packetCount = this->_packetsReceived.size();

        for (size_t i = 0; i < packetCount; ++i) {
            packetsToProcess.push_back(this->_packetsReceived.front());
            this->_packetsReceived.pop_front();
        }

        //this->_gameEngine->coordinator->processPackets(packetsToProcess);       // process all received packets with the coordinator -> NetworkManager -> EntityManager

        // Poll inputs every tick
        //this->_gameEngine->input->poll();                                       // poll inputs from engine input system (SFML)
        //this->_gameEngine->coordinator->processInputs();                        // process inputs w coordinator -> Redermanager -> (we user SFML for input handling)

        // Update game state every tick
        //this->_gameEngine->coordinator->update(deltaTime, currentTick);         // engine -> coordinator -> ecs (all systems update)

        // Build and send packets based on tick intervals
        std::vector<common::protocol::Packet> outgoingPackets;

        bool shouldSendHeartbeat = (currentTick - lastHeartbeatTick) >= HEARTBEAT_TICK_INTERVAL;
        bool shouldSendInputs = (currentTick - lastInputSendTick) >= INPUT_SEND_TICK_INTERVAL;

        if (shouldSendHeartbeat || shouldSendInputs) {
            //this->_gameEngine->coordinator->buildPacketBasedOnStatus(           // build packets to send to server based on game state and last heartbeat and time
            //    outgoingPackets,
            //    currentTick,
            //    shouldSendHeartbeat,
            //    shouldSendInputs
            //);

            for (const auto& packet : outgoingPackets) {
                this->_packetsToSend.push_back(packet);
            }

            if (shouldSendHeartbeat) {
                lastHeartbeatTick = currentTick;
            }
            if (shouldSendInputs) {
                lastInputSendTick = currentTick;
            }
        }
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <server_ip>" << "<port>" << "<player_name>" << std::endl;
        return 1;
    }

    RTypeClient client;

    // Initialize the client with server IP, port, and player name
    client.init(argv[1], std::stoi(argv[2]), argv[3]);

    // Start the client
    client.run();

    return 0;
}