/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RTypeClient
*/

#include <client/RTypeClient.hpp>
#include <string.h>
#include <game/utils/ClientUtils.hpp>
#include <thread>
#include <chrono>
#include <iostream>
#include <common/protocol/Protocol.hpp>
#include <common/constants/defines.hpp>
#include <client/network/ClientNetworkManager.hpp>
#include <common/logger/Logger.hpp>
#include <common/error/Error.hpp>
#include <client/menu/ClientMenu.hpp>

RTypeClient::RTypeClient() : _isRunning(false)
{
}

RTypeClient::~RTypeClient()
{
}

void RTypeClient::init(const char* serverIp, uint16_t port, std::string playerName)
{
    _networkManager = std::make_unique<client::network::ClientNetworkManager>(serverIp, port, this);

    // will init the engine && coordinator
    _game = std::make_unique<Game>(Game::Type::CLIENT);

    // create menu using engine and coordinator inside the game
    auto coordinator = _game->getCoordinator();
    auto engine = coordinator->getEngine();
    auto menu = std::make_shared<ClientMenu>(engine, coordinator);

    // set menu in the game
    _game->setMenu(menu);
    
    // Set up callback for level complete (client-side) - display score menu
    coordinator->_levelCompleteCallback = [this](uint32_t finalScore) {
        LOG_INFO("Level complete! Final score: {} - Showing score menu", finalScore);
        _game->showScoreMenu(finalScore);
    };

    // 4. init main menu content
    menu->createMainMenu();

    _playerName = playerName;
    _isRunning = false;
    _isConnected = false;
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

        while (_isRunning) {
                // Feed incoming packets to the game and run a game step
                auto incomingPackets = _networkManager->fetchIncoming();
                for (const auto &entry : incomingPackets) {
                    if (_game) {
                        _game->addIncomingPacket(entry);
                    }
                }

                if (_game) {
                    if (!_game->runGameLoop()) { // dedans on va process packet + update + create packet + render
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
                        _networkManager->queueOutgoing(out.first);
                    }
                }
            //std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

    networkThread.join();
}

void RTypeClient::stop()
{
    LOG_INFO("Stopping R-Type Client...");
    _isRunning = false;
    if (_networkManager) {
        _networkManager->stop();
    }
    LOG_INFO("Client stopped successfully");
}
