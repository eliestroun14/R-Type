/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RTypeClient
*/

#include <client/RTypeClient.hpp>
#include <string.h>
#include <arpa/inet.h>
#include <client/utils/ClientUtils.hpp>
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
    _networkManager = std::make_unique<client::network::ClientNetworkManager>(serverIp, port);
    this->_gameEngine = std::make_shared<gameEngine::GameEngine>();             // TODO

    _packetsReceived.clear();
    _packetsToSend.clear();

    _playerName = playerName;
    _isRunning = false;
    // Additional initialization code can be added here
}

void RTypeClient::run()
{
    _isRunning = true;
    _networkManager->start();

    std::cout << "client run" << std::endl;
    
    std::thread networkThread(&RTypeClient::networkLoop, this);
    
    this->_gameEngine->init();
    this->_gameEngine->initRender();
    
    // Keep gameLoop in main thread for OpenGL context to work properly
    this->gameLoop();

    networkThread.join();
}

void RTypeClient::stop()
{
    _isRunning = false;
    if (_networkManager) {
        _networkManager->stop();
    }
}

void RTypeClient::networkLoop()
{
    while (_isRunning) {
        auto packets = _networkManager->fetchIncoming();
        for (const auto& entry : packets) {
            _packetsReceived.push_back(entry.packet);
        }

        while (!_packetsToSend.empty()) {
            _networkManager->queueOutgoing(_packetsToSend.front());
            _packetsToSend.pop_front();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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

        // this->_gameEngine->coordinator->processPackets(packetsToProcess);       // process all received packets with the coordinator -> PacketManager -> EntityManager

        // // Poll inputs every tick
        // this->_gameEngine->input->poll();                                       // poll inputs from engine input system (SFML)
        // this->_gameEngine->coordinator->processInputs();                        // process inputs w coordinator -> Redermanager -> (we user SFML for input handling)



        // Update game state every tick
        this->_gameEngine->process(deltaTime, NetworkType::NETWORK_TYPE_STANDALONE);         // engine -> coordinator -> ecs (all systems update)

        // Build and send packets based on tick intervals
        std::vector<common::protocol::Packet> outgoingPackets;

        //this->_gameEngine->coordinator->buildPacketBasedOnStatus(           // build packets to send to server based on game state and last heartbeat and time
        //    outgoingPackets,
        //    currentTick,
        //    NETWORK_TYPE_CLIENT,
        //    lastHeartbeatTick,
        //    lastInputSendTick
        //);

        for (const auto& packet : outgoingPackets) {
            this->_packetsToSend.push_back(packet);
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