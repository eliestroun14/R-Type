#include <server/core/Server.hpp>
#include <iostream>
#include <cstring>

namespace server {

Server::Server(const ServerConfig& config)
    : _config(config)
    , _isRunning(false)
    , _networkManager(std::make_unique<server::network::ServerNetworkManager>(_config.port, _config.maxPlayers))
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
    _gameEngine = std::make_shared<gameEngine::GameEngine>();
    _networkManager->start();

    std::cout << "server run" << std::endl;

    std::thread networkThread(&server::network::ServerNetworkManager::run, _networkManager.get());

    this->gameLoop();
    networkThread.join();

}

void Server::stop() {
    _isRunning = false;
    if (_networkManager) {
        _networkManager->stop();
    }
}

void Server::gameLoop() {

    _gameEngine->init();
    
    auto next = std::chrono::steady_clock::now();
    const std::chrono::milliseconds tick(std::chrono::milliseconds(TICK_RATE));

    uint64_t currentTick = 0;
    uint64_t lastHeartbeatTick = 0;

    while (_isRunning) {

        next += tick;
        std::this_thread::sleep_until(next);

        currentTick++;

        // Send heartbeat every HEARTBEAT_INTERVAL_TICKS
        if (currentTick - lastHeartbeatTick >= HEARTBEAT_INTERVAL) {
            sendHeartbeat();
            checkClientTimeouts();
            lastHeartbeatTick = currentTick;
        }

        // Calculate delta time in milliseconds
        float deltaTime = static_cast<float>(TICK_RATE);

        // Process received packets
        std::vector<common::protocol::Packet> packetsToProcess;
        auto incomingPackets = _networkManager->fetchIncoming();
        for (const auto& entry : incomingPackets) {
            packetsToProcess.push_back(entry.packet);
        }

        //this->_gameEngine->coordinator->processPackets(packetsToProcess);       // process all received packets with the coordinator -> PacketManager -> EntityManager

        // Update game state every tick
        _gameEngine->process(deltaTime, NetworkType::NETWORK_TYPE_STANDALONE);

        // Build and send packets based on tick intervals
        std::vector<common::protocol::Packet> outgoingPackets;

        //this->_gameEngine->coordinator->buildPacketBasedOnStatus(           // build packets to send to server based on game state and last heartbeat and time
        //    outgoingPackets,
        //    currentTick,
        //    lastHeartbeatTick,
        //    NETWORK_TYPE_SERVER
        //);

        for (const auto& packet : outgoingPackets) {
            _networkManager->queueOutgoing(packet);
        }
    }
}

void Server::sendHeartbeat()
{
    common::protocol::Packet heartbeat(static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEARTBEAT));
    
    // Broadcast heartbeat to all connected clients
    _networkManager->queueOutgoing(heartbeat);
}

void Server::checkClientTimeouts()
{
    // TODO: Implement client timeout checking
    // Track last heartbeat received from each client
    // Disconnect clients that haven't responded within timeout period
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
