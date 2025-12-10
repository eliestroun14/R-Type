#include "../include/server/core/Server.hpp"
#include <iostream>
#include <cstring>

namespace server {

Server::Server(const ServerConfig& config)
    : _config(config)
    , _isRunning(false)
    , _networkManager(std::make_unique<server::network::ServerNetworkManager>(_config.port, _config.maxPlayers))
    , _packetsReceived()
    , _packetsToSend()
{
}

Server::~Server() {
    stop();
}

bool Server::init() {
    return true;
}

void Server::run() {
    _isRunning = true;
    _networkManager->start();

    std::thread networkThread(&Server::networkLoop, this);

    std::thread gameThread(&Server::gameLoop, this);

    networkThread.join();
    gameThread.join();

}

void Server::stop() {
    _isRunning = false;
    if (_networkManager) {
        _networkManager->stop();
    }
}

void Server::networkLoop() {

    while (_isRunning) {

        auto packets = _networkManager->fetchIncoming();
        for (const auto& entry : packets) {
            _packetsReceived.push_back(entry.packet);
        }

        while (!_packetsToSend.empty()) {
            _networkManager->queueOutgoing(_packetsToSend.front());
            _packetsToSend.pop_front();
        }
    }
}

void Server::gameLoop() {

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

        //this->_gameEngine->coordinator->processPackets(packetsToProcess);       // process all received packets with the coordinator -> PacketManager -> EntityManager

        // Update game state every tick
        //this->_gameEngine->coordinator->update(deltaTime, currentTick);         // engine -> coordinator -> ecs (all systems update)

        // Build and send packets based on tick intervals
        std::vector<common::protocol::Packet> outgoingPackets;

        //this->_gameEngine->coordinator->buildPacketBasedOnStatus(           // build packets to send to server based on game state and last heartbeat and time
        //    outgoingPackets,
        //    currentTick,
        //    lastHeartbeatTick,
        //    lastInputSendTick,
        //    NETWORK_TYPE_SERVER
        //);

        for (const auto& packet : outgoingPackets) {
            this->_packetsToSend.push_back(packet);
        }
    }
}

} // namespace server

int main(int argc, char const *argv[])
{
    server::ServerConfig config;
    config.port = argc > 1 ? static_cast<uint16_t>(std::stoi(argv[1])) : 4242;
    config.maxPlayers = argc > 2 ? static_cast<uint32_t>(std::stoi(argv[2])) : 16;
    config.tickRate = argc > 3 ? static_cast<uint32_t>(std::stoi(argv[3])) : 60;

    server::Server server(config);
    if (!server.init()) {
        std::cerr << "[Server] Failed to initialize." << std::endl;
        return 1;
    }

    server.run();

    return 0;
}
