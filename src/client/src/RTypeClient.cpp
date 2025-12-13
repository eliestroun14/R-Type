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

    _packetsReceived.clear();
    _packetsToSend.clear();

    _playerName = playerName;
    _isRunning = false;
    tickCount = 0;
    // Additional initialization code can be added here
}

void RTypeClient::run()
{
    _isRunning = true;
    _networkManager->start();

    // Send initial connection request
    sendConnectionRequest();

    std::thread networkThread(&RTypeClient::networkLoop, this);
    
    this->_gameEngine->init();
    this->_gameEngine->initRender();
    
    // Keep gameLoop in main thread for OpenGL context to work properly
    this->gameLoop();

    networkThread.join();
}

void RTypeClient::stop()
{
    sendDisconnect(static_cast<uint8_t>(protocol::DisconnectReasons::REASON_NORMAL_DISCONNECT));
    _isRunning = false;
    if (_networkManager) {
        _networkManager->stop();
    }
}

void RTypeClient::networkLoop()
{
    auto next = std::chrono::steady_clock::now();
    const std::chrono::milliseconds tick(std::chrono::milliseconds(TICK_RATE));
    
    auto startEpoch = std::chrono::steady_clock::now();
    uint64_t lastHeartbeatTime = 0;

    while (_isRunning) {
        // Calculate time since epoch in milliseconds
        auto currentTime = std::chrono::steady_clock::now();
        uint64_t elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startEpoch).count();

        // Handle heartbeat every HEARTBEAT_INTERVAL milliseconds
        if (elapsedMs - lastHeartbeatTime >= HEARTBEAT_INTERVAL) {
            sendHeartbeat();
            lastHeartbeatTime = elapsedMs;
        }

        // Fetch incoming packets
        auto packets = _networkManager->fetchIncoming();
        for (const auto& entry : packets) {
            _packetsReceived.push_back(entry.packet);
        }

        // Send outgoing packets
        while (!_packetsToSend.empty()) {
            _networkManager->queueOutgoing(_packetsToSend.front());
            _packetsToSend.pop_front();
        }
    }
}

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
        size_t packetCount = this->_packetsReceived.size();

        for (size_t i = 0; i < packetCount; ++i) {
            packetsToProcess.push_back(this->_packetsReceived.front());
            this->_packetsReceived.pop_front();
        }

        // this->_gameEngine->coordinator->processPackets(packetsToProcess);       // process all received packets with the coordinator -> PacketManager -> EntityManager

        // // Poll inputs every tick
        // this->_gameEngine->input->poll();                                       // poll inputs from engine input system (SFML)
        this->_gameEngine->processInput();                        // process inputs w coordinator -> Redermanager -> (we user SFML for input handling)



        // Update game state every tick
        this->_gameEngine->process(deltaTime, NetworkType::NETWORK_TYPE_STANDALONE);         // engine -> coordinator -> ecs (all systems update) elapsedMS

        // Build and send packets based on tick intervals
        std::vector<common::protocol::Packet> outgoingPackets;

        //this->_gameEngine->coordinator->buildPacketBasedOnStatus(           // build packets to send to server based on game state and elapsed time
        //    outgoingPackets,
        //    elapsedMs,
        //    NETWORK_TYPE_CLIENT,
        //    lastInputSendTime
        //);

        for (const auto& packet : outgoingPackets) {
            this->_packetsToSend.push_back(packet);
        }
    }
}

void RTypeClient::sendConnectionRequest()
{
    common::protocol::Packet connectPacket(static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_CONNECT), 0, tickCount, TIMESTAMP);

    _networkManager->queueOutgoing(connectPacket);
    std::cout << "[RTypeClient] Connection request sent" << std::endl;
}

void RTypeClient::sendHeartbeat()
{
    //common::protocol::Packet heartbeat(static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEARTBEAT));

    common::protocol::PacketHeader header;
    header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEARTBEAT);
    header.sequence_number = tickCount;
    header.timestamp = TIMESTAMP;
    header.flags = 0;

    protocol::ClientConnect payload;
    payload.header = header;
    payload.protocol_version = PROTOCOL_VERSION;
    std::strncpy(payload.player_name, _playerName.c_str(), sizeof(payload.player_name) - 1);
    payload.player_name[sizeof(payload.player_name) - 1] = '\0'; // Ensure null-termination
    payload.client_id = _selfId;

    common::protocol::Packet heartbeat(header, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&payload), reinterpret_cast<uint8_t*>(&payload) + sizeof(payload)));
    _networkManager->queueOutgoing(heartbeat);
    std::cout << "[RTypeClient] Heartbeat sent" << std::endl;
}

void RTypeClient::sendDisconnect(uint8_t reason)
{
    common::protocol::PacketHeader header;
    header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_DISCONNECT);
    header.sequence_number = tickCount;
    header.timestamp = TIMESTAMP;
    header.flags = 0;

    protocol::ClientDisconnect payload;
    payload.header = header;
    payload.client_id = _selfId;
    payload.reason = reason;

    common::protocol::Packet disconnect(header, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&payload), reinterpret_cast<uint8_t*>(&payload) + sizeof(payload)));
    _networkManager->queueOutgoing(disconnect);
    std::cout << "[RTypeClient] Disconnect packet sent" << std::endl;
}

void RTypeClient::sendAck(uint32_t acked_sequence, uint32_t received_timestamp)
{
    common::protocol::PacketHeader header;
    header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_ACK);
    header.sequence_number = tickCount;
    header.timestamp = TIMESTAMP;
    header.flags = 0;

    protocol::Acknowledgment payload;
    payload.header = header;
    payload.client_id = _selfId;
    payload.acked_sequence = acked_sequence;
    payload.received_timestamp = received_timestamp;


    common::protocol::Packet ack(header, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&payload), reinterpret_cast<uint8_t*>(&payload) + sizeof(payload)));
    _networkManager->queueOutgoing(ack);
    std::cout << "[RTypeClient] ACK packet sent" << std::endl;
}

void RTypeClient::sendPing()
{
    common::protocol::PacketHeader header;
    header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_PING);
    header.sequence_number = tickCount;
    header.timestamp = TIMESTAMP;
    header.flags = 0;

    protocol::Ping payload;
    payload.header = header;
    payload.client_id = _selfId;
    payload.client_timestamp = TIMESTAMP;

    common::protocol::Packet ping(header, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&payload), reinterpret_cast<uint8_t*>(&payload) + sizeof(payload)));
    _networkManager->queueOutgoing(ping);
    std::cout << "[RTypeClient] Ping packet sent" << std::endl;
}

uint32_t RTypeClient::calculateLatency(uint32_t ping_sent_time , uint32_t ping_received_time)
{
    return ping_received_time - ping_sent_time;
}