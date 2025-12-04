/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** main - R-Type Server Entry Point
*/

#include <iostream>
#include <string>
#include <csignal>
#include <memory>
#include "Server.hpp"

// Global server pointer for signal handling
std::unique_ptr<server::Server> g_server;

void signalHandler(int signal) {
    std::cout << "\n[Main] Received signal " << signal << ", shutting down..." << std::endl;
    if (g_server) {
        g_server->stop();
    }
}

void printUsage(const char* programName) {
    std::cout << "R-Type Server - A multiplayer space shooter" << std::endl;
    std::cout << "\nUsage: " << programName << " [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -p, --port <port>        Server port (default: 4242)" << std::endl;
    std::cout << "  -m, --max-players <num>  Maximum number of players (default: 16)" << std::endl;
    std::cout << "  -t, --tickrate <rate>    Server tick rate in Hz (default: 60)" << std::endl;
    std::cout << "  -h, --help               Display this help message" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  " << programName << " --port 4242 --max-players 8" << std::endl;
}

int main(int argc, char** argv) {
    // Configuration par défaut
    server::ServerConfig config;

    // Parser les arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                config.port = static_cast<uint16_t>(std::stoi(argv[++i]));
            } else {
                std::cerr << "Error: " << arg << " requires a value" << std::endl;
                return 1;
            }
        }
        else if (arg == "-m" || arg == "--max-players") {
            if (i + 1 < argc) {
                config.maxPlayers = static_cast<uint32_t>(std::stoi(argv[++i]));
            } else {
                std::cerr << "Error: " << arg << " requires a value" << std::endl;
                return 1;
            }
        }
        else if (arg == "-t" || arg == "--tickrate") {
            if (i + 1 < argc) {
                config.tickRate = static_cast<uint32_t>(std::stoi(argv[++i]));
            } else {
                std::cerr << "Error: " << arg << " requires a value" << std::endl;
                return 1;
            }
        }
        else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    // Afficher le banner
    std::cout << R"(
    ____        ______                 
   / __ \      /_  __/_  ______  ___  
  / /_/ /_____  / / / / / / __ \/ _ \ 
 / _, _/_____/ / / / /_/ / /_/ /  __/ 
/_/ |_|       /_/  \__, / .___/\___/  
                  /____/_/             
    )" << std::endl;
    
    std::cout << "R-Type Multiplayer Server" << std::endl;
    std::cout << "Version 1.0 - MVP (Étape 1: Accepter des connexions)" << std::endl;
    std::cout << std::endl;

    // Créer le serveur
    g_server = std::make_unique<server::Server>(config);

    // Installer les gestionnaires de signaux
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Initialiser le serveur
    if (!g_server->initialize()) {
        std::cerr << "[Main] Failed to initialize server" << std::endl;
        return 1;
    }

    // Démarrer le serveur
    if (!g_server->start()) {
        std::cerr << "[Main] Failed to start server" << std::endl;
        return 1;
    }

    // Lancer la boucle principale (bloquante)
    g_server->run();

    // Nettoyage
    g_server.reset();

    std::cout << "[Main] Server shutdown complete" << std::endl;
    return 0;
}