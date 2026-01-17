/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** main
*/

#include <client/RTypeClient.hpp>
#include <iostream>
#include <string>
#include <map>
#include <common/logger/Logger.hpp>
#include <common/error/Error.hpp>

struct ClientConfig {
    std::string serverIp = "127.0.0.1";
    uint16_t port = 4242;
    std::string playerName = "Player";
};

void printHelp(const char* programName) {
    std::cout << "R-Type Client - Usage:\n\n";
    std::cout << "  " << programName << " [OPTIONS]\n\n";
    std::cout << "OPTIONS:\n";
    std::cout << "  -h, --help              Display this help message\n";
    std::cout << "  -h, --host <ip>         Set host IP address (default: 127.0.0.1)\n";
    std::cout << "  -p, --port <value>      Set server port (default: 4242)\n";
    std::cout << "  -n, --name <name>       Set player name (default: Player)\n\n";
    std::cout << "EXAMPLES:\n";
    std::cout << "  " << programName << " -h 192.168.1.100 -p 8080 -n Player1\n";
    std::cout << "  " << programName << " --host localhost --port 4242 --name John\n";
    std::cout << "  " << programName << " -h 127.0.0.1 -n Alice\n\n";
}

// Parse command line arguments
static bool parseArguments(int argc, char** argv, ClientConfig& config) {
    std::map<std::string, std::string> args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Check for help flag
        if (arg == "-h" || arg == "--help") {
            printHelp(argv[0]);
            return false;
        }

        // Parse key-value arguments
        if (arg == "-h" || arg == "--host") {
            if (i + 1 < argc) {
                args["host"] = argv[++i];
            } else {
                throw Error(ErrorType::ConfigurationError, 
                    std::string("Argument ") + arg + " requires a value");
            }
        }
        else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                args["port"] = argv[++i];
            } else {
                throw Error(ErrorType::ConfigurationError, 
                    std::string("Argument ") + arg + " requires a value");
            }
        }
        else if (arg == "-n" || arg == "--name") {
            if (i + 1 < argc) {
                args["name"] = argv[++i];
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
        if (args.find("host") != args.end()) {
            config.serverIp = args["host"];
            if (config.serverIp.empty()) {
                throw Error(ErrorType::ConfigurationError, 
                    "Host IP cannot be empty");
            }
        }

        if (args.find("port") != args.end()) {
            int port = std::stoi(args["port"]);
            if (port < 1 || port > 65535) {
                throw Error(ErrorType::ConfigurationError, 
                    "Port must be between 1 and 65535");
            }
            config.port = static_cast<uint16_t>(port);
        }

        if (args.find("name") != args.end()) {
            config.playerName = args["name"];
            if (config.playerName.empty()) {
                throw Error(ErrorType::ConfigurationError, 
                    "Player name cannot be empty");
            }
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

int main(int argc, char** argv)
{
    // Initialize logger first
    logger::Logger::setup(
        logger::LogLevel::Debug,
        "client.log",
        {},
        false,
        true,
        false,
        true
    );

    try {
        // Default configuration
        ClientConfig config;

        // Parse command line arguments
        if (!parseArguments(argc, argv, config)) {
            return 0; // Return 0 for help display (not an error)
        }

        // Display client configuration
        LOG_INFO("=== R-Type Client ===");
        LOG_INFO("Server: {}:{}", config.serverIp, config.port);
        LOG_INFO("Player: {}", config.playerName);

        RTypeClient client;

        // Initialize the client with server IP, port, and player name
        client.init(config.serverIp.c_str(), config.port, config.playerName);

        // Start the client
        client.run();

        LOG_INFO("Client exited normally");

    } catch (const Error& e) {
        LOG_ERROR("Client error: {}", e.what());
        logger::Logger::getInstance().shutdown();
        return 84;
    } catch (const std::exception& e) {
        LOG_CRITICAL("Unexpected error: {}", e.what());
        logger::Logger::getInstance().shutdown();
        return 84;
    }

    logger::Logger::getInstance().shutdown();
    return 0;
}
