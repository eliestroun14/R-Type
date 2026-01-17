#include <server/core/Server.hpp>
#include <game/Game.hpp>
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
    , _game(std::make_unique<Game>(Game::Type::SERVER))
{
}

Server::~Server() {
    stop();
}

bool Server::init() {
    // Set max players for level start condition
    if (_game) {
        _game->setMaxPlayers(_config.maxPlayers);
        LOG_INFO("Server: Set max players to {} for level start", _config.maxPlayers);
    }

    // Set up callback for when players connect
    _networkManager->setOnPlayerConnectedCallback([this](uint32_t playerId) {
        if (_game) {
            _game->onPlayerConnected(playerId);
        }
    });

    return true;
}

void Server::run() {
    _isRunning = true;
    _networkManager->start();

    LOG_INFO("Server running on port {}", _config.port);

    std::thread networkThread(&server::network::ServerNetworkManager::run, _networkManager.get());
    networkThread.detach(); // Detach the network thread to allow independent execution

    while (_isRunning && !g_shutdownRequested.load()) {
        // Basic game processing: feed incoming packets and run game loop
        auto incomingPackets = _networkManager->fetchIncoming();
        if (!incomingPackets.empty()) {
            LOG_DEBUG("Server: fetched {} incoming packets from network", incomingPackets.size());
        }
        for (const auto &entry : incomingPackets) {
            if (_game) {
                LOG_DEBUG("Server: forwarding packet type={} to game", static_cast<int>(entry.packet.header.packet_type));
                _game->addIncomingPacket(entry);
            }
        }

        if (_game) {
            if (!_game->runGameLoop()) {
                stop();
                break;
            }
        }
        // Forward any outgoing packets from Game to the network manager
        if (_game) {
            while (true) {
                auto maybeOut = _game->popOutgoingPacket();
                if (!maybeOut.has_value())
                    break;
                auto &out = maybeOut.value();
                _networkManager->queueOutgoing(out.first, out.second);
            }
        }
        
        // Small sleep to prevent busy-waiting and allow time to accumulate for fixed timestep
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (g_shutdownRequested.load()) {
        LOG_INFO("Shutdown requested, stopping gracefully...");
    }
}

void Server::stop() {
    _isRunning = false;
    if (_networkManager) {
        _networkManager->stop();
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
        logger::LogLevel::Debug,
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
