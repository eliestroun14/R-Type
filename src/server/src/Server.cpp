#include <server/core/Server.hpp>
#include <iostream>
#include <cstring>
#include <csignal>
#include <string>
#include <map>
#include <common/logger/Logger.hpp>
#include <common/error/Error.hpp>

namespace server {

// Global flag for signal handling
static std::atomic<bool> g_shutdownRequested{false};
static Server* g_serverInstance = nullptr;

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
    _gameEngine->init();  // Initialize engine BEFORE network manager starts
    _networkManager->setGameEngine(_gameEngine);
    _networkManager->start();

    LOG_INFO("Server running on port {}", _config.port);

    std::thread networkThread(&server::network::ServerNetworkManager::run, _networkManager.get());
    networkThread.detach(); // Detach the network thread to allow independent execution

    this->gameLoop();
    
    // Ensure network manager is stopped
    if (_networkManager) {
        _networkManager->stop();
    }
    
    // Give network thread a moment to process the stop signal
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

}

void Server::stop() {
    _isRunning = false;
    if (_networkManager) {
        _networkManager->stop();
    }
}

void Server::gameLoop() {

    // Engine already initialized in run()
    
    auto next = std::chrono::steady_clock::now();
    const std::chrono::milliseconds tick(std::chrono::milliseconds(TICK_RATE));

    uint64_t currentTick = 0;
    uint64_t lastHeartbeatTick = 0;

    while (_isRunning && !g_shutdownRequested.load()) {

        next += tick;
        std::this_thread::sleep_until(next);

        currentTick++;

        // Calculate delta time in milliseconds
        float deltaTime = static_cast<float>(TICK_RATE);

        // Process received packets
        std::vector<common::protocol::Packet> packetsToProcess;
        auto incomingPackets = _networkManager->fetchIncoming();
        for (const auto& entry : incomingPackets) {
            packetsToProcess.push_back(entry.packet);
        }

        // Update game state every tick
         //FIXME: game engine has been refactor so need to be changed
        // _gameEngine->process(deltaTime, NetworkType::NETWORK_TYPE_SERVER, packetsToProcess, currentTick * TICK_RATE);

        // Build and send packets based on tick intervals
        std::vector<common::protocol::Packet> outgoingPackets;


         //FIXME: game engine has been refactor so need to be changed
        // this->_gameEngine->buildPacketBasedOnStatus(           // build packets to send to server based on game state and elapsed time
        //     NetworkType::NETWORK_TYPE_CLIENT,
        //     currentTick * TICK_RATE,
        //     outgoingPackets
        // );

        for (const auto& packet : outgoingPackets) {
            _networkManager->queueOutgoing(packet);
        }
    }
    
    if (g_shutdownRequested.load()) {
        LOG_INFO("Shutdown requested, stopping gracefully...");
    }
}

} // namespace server

// Signal handler
static void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        LOG_WARN("Received shutdown signal ({})", signal == SIGINT ? "SIGINT" : "SIGTERM");
        server::g_shutdownRequested.store(true);
        if (server::g_serverInstance) {
            server::g_serverInstance->stop();
        }
    }
}

// Print help message
static void printHelp(const char* programName) {
    std::cout << "R-Type Server - Usage:\n\n";
    std::cout << "  " << programName << " [OPTIONS]\n\n";
    std::cout << "OPTIONS:\n";
    std::cout << "  -h, --help              Display this help message\n";
    std::cout << "  -p, --port <value>      Set server port (default: 4242)\n";
    std::cout << "  -mp, --maxplayer <value> Set maximum number of players (default: 16)\n";
    std::cout << "  -tr, --tickrate <value>  Set server tick rate in Hz (default: 60)\n\n";
    std::cout << "EXAMPLES:\n";
    std::cout << "  " << programName << " -p 8080\n";
    std::cout << "  " << programName << " --port 8080 --maxplayer 32 --tickrate 120\n";
    std::cout << "  " << programName << " -p 5000 -mp 8\n\n";
}

// Parse command line arguments
static bool parseArguments(int argc, char const *argv[], server::ServerConfig& config) {
    std::map<std::string, std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Check for help flag
        if (arg == "-h" || arg == "--help") {
            printHelp(argv[0]);
            return false;
        }

        // Parse key-value arguments
        if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                args["port"] = argv[++i];
            } else {
                throw Error(ErrorType::ConfigurationError, 
                    std::string("Argument ") + arg + " requires a value");
            }
        }
        else if (arg == "-mp" || arg == "--maxplayer") {
            if (i + 1 < argc) {
                args["maxplayer"] = argv[++i];
            } else {
                throw Error(ErrorType::ConfigurationError, 
                    std::string("Argument ") + arg + " requires a value");
            }
        }
        else if (arg == "-tr" || arg == "--tickrate") {
            if (i + 1 < argc) {
                args["tickrate"] = argv[++i];
            } else {
                throw Error(ErrorType::ConfigurationError, 
                    std::string("Argument ") + arg + " requires a value");
            }
        }
        else {
            throw Error(ErrorType::ConfigurationError, 
                std::string("Unknown argument: ") + arg);
        }
    }
    
    // Apply parsed values
    try {
        if (args.find("port") != args.end()) {
            int port = std::stoi(args["port"]);
            if (port < 1 || port > 65535) {
                throw Error(ErrorType::ConfigurationError, 
                    "Port must be between 1 and 65535");
            }
            config.port = static_cast<uint16_t>(port);
        }

        if (args.find("maxplayer") != args.end()) {
            int maxPlayers = std::stoi(args["maxplayer"]);
            if (maxPlayers < 1 || maxPlayers > 1000) {
                throw Error(ErrorType::ConfigurationError, 
                    "Max players must be between 1 and 1000");
            }
            config.maxPlayers = static_cast<uint32_t>(maxPlayers);
        }

        if (args.find("tickrate") != args.end()) {
            int tickRate = std::stoi(args["tickrate"]);
            if (tickRate < 1 || tickRate > 1000) {
                throw Error(ErrorType::ConfigurationError, 
                    "Tick rate must be between 1 and 1000 Hz");
            }
            config.tickRate = static_cast<uint32_t>(tickRate);
        }
    } catch (const std::invalid_argument& e) {
        throw Error(ErrorType::ConfigurationError, 
            std::string("Invalid numeric argument value: ") + e.what());
    } catch (const std::out_of_range& e) {
        throw Error(ErrorType::ConfigurationError, 
            std::string("Argument value out of range: ") + e.what());
    }
    
    return true;
}

int main(int argc, char const *argv[])
{
    // Initialize logger first
    logger::Logger::setup(
        logger::LogLevel::Info,
        "server.log",
        {},
        false,
        true,
        false,
        true
    );
    
    // Set up signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    try {
        // Default configuration
        server::ServerConfig config;

        // Parse command line arguments
        if (!parseArguments(argc, argv, config)) {
            return 0; // Return 0 for help display (not an error)
        }

        // Display server configuration
        LOG_INFO("Starting R-Type Server");
        LOG_INFO("Port: {}", config.port);
        LOG_INFO("Max Players: {}", config.maxPlayers);
        LOG_INFO("Tick Rate: {} Hz", config.tickRate);
        LOG_INFO("Press Ctrl+C to stop");

        server::Server server(config);
        server::g_serverInstance = &server;  // Set global instance for signal handler

        if (!server.init()) {
            LOG_CRITICAL("Failed to initialize server");
            server::g_serverInstance = nullptr;
            return 84;
        }

        server.run();

        server::g_serverInstance = nullptr;  // Clear global instance
        LOG_INFO("Server stopped successfully");

    } catch (const Error& e) {
        LOG_ERROR("Server error: {}", e.what());
        server::g_serverInstance = nullptr;
        return 84;
    } catch (const std::exception& e) {
        LOG_CRITICAL("Unexpected error: {}", e.what());
        server::g_serverInstance = nullptr;
        return 84;
    }

    logger::Logger::getInstance().shutdown();
    return 0;
}
