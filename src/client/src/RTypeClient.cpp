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
#include <common/protocol/Protocol.hpp>
#include <common/constants/defines.hpp>
#include <client/network/ClientNetworkManager.hpp>
#include <engine/utils/Logger.hpp>
#include <common/error/Error.hpp>





RTypeClient::RTypeClient() : _selfId(0), _isRunning(false)
{
}

RTypeClient::~RTypeClient()
{
}

void RTypeClient::init(const char* serverIp, uint16_t port, std::string playerName)
{
    _networkManager = std::make_unique<client::network::ClientNetworkManager>(serverIp, port, this);
    this->_gameEngine = std::make_shared<gameEngine::GameEngine>();             // TODO

    _playerName = playerName;
    _isRunning = false;
    tickCount = 0;
    _isConnected = false;
    // Additional initialization code can be added here
}

void RTypeClient::run()
{
    _isRunning = true;
    _networkManager->start();

    LOG_INFO("Starting R-Type Client");
    
    // Send initial connection request
    _networkManager->sendConnectionRequest();

    std::thread networkThread(&client::network::ClientNetworkManager::run, _networkManager.get());

    LOG_INFO("Waiting for connection to server...");
    
    // Wait for connection to be established
    while (!isConnected() && _isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (isConnected()) {
        LOG_INFO("Connected to server! Starting game...");
        this->_gameEngine->init();
        this->_gameEngine->initRender();
        
        // Keep gameLoop in main thread for OpenGL context to work properly
        this->gameLoop();
    } else {
        LOG_WARN("Failed to connect to server");
    }
    
    networkThread.join();
}

void RTypeClient::stop()
{
    LOG_INFO("Stopping R-Type Client...");
    if (_networkManager) {
        _networkManager->sendDisconnect(static_cast<uint8_t>(protocol::DisconnectReasons::REASON_NORMAL_DISCONNECT));
    }
    _isRunning = false;
    if (_networkManager) {
        _networkManager->stop();
    }
    LOG_INFO("Client stopped successfully");
}

//void RTypeClient::networkLoop()
//{
//    // Directly call the network manager's run method
//    // This handles heartbeats, packet reception, packet sending, and network-level packet handling
//    if (_networkManager) {
//        _networkManager->run();
//    }
//}

void RTypeClient::gameLoop()
{
    auto next = std::chrono::steady_clock::now();
    const std::chrono::milliseconds tick(std::chrono::milliseconds(TICK_RATE));

    auto startEpoch = std::chrono::steady_clock::now();
    uint64_t lastInputSendTime = 0;

    while (_isRunning) {

        next += tick;
        std::this_thread::sleep_until(next);
        tickCount++;

        // Calculate time since epoch in milliseconds
        auto currentTime = std::chrono::steady_clock::now();
        uint64_t elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startEpoch).count();

        // Calculate delta time in milliseconds
        float deltaTime = static_cast<float>(TICK_RATE);

        // Process received packets
        std::vector<common::protocol::Packet> packetsToProcess;
        
        // Fetch packets from network manager
        auto incomingPackets = _networkManager->fetchIncoming();
        for (const auto& entry : incomingPackets) {
            packetsToProcess.push_back(entry.packet);
        }

        // this->_gameEngine->coordinator->processPackets(packetsToProcess);       // process all received packets with the coordinator -> PacketManager -> EntityManager

        // // Poll inputs every tick
        // this->_gameEngine->input->poll();                                       // poll inputs from engine input system (SFML)
        this->_gameEngine->processInput();                        // process inputs w coordinator -> Redermanager -> (we user SFML for input handling)



        // Update game state every tick
        this->_gameEngine->process(deltaTime, NetworkType::NETWORK_TYPE_CLIENT);         // engine -> coordinator -> ecs (all systems update) elapsedMS

        // Build and send packets based on tick intervals
        std::vector<common::protocol::Packet> outgoingPackets;

        //this->_gameEngine->coordinator->buildPacketBasedOnStatus(           // build packets to send to server based on game state and elapsed time
        //    outgoingPackets,
        //    elapsedMs,
        //    NETWORK_TYPE_CLIENT,
        //    lastInputSendTime
        //);

        // Send packets directly to network manager
        for (const auto& packet : outgoingPackets) {
            _networkManager->queueOutgoing(packet);
        }
    }
}

uint32_t RTypeClient::calculateLatency(uint32_t ping_sent_time , uint32_t ping_received_time)
{
    return ping_received_time - ping_sent_time;
}