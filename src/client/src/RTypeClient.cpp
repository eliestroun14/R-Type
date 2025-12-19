/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RTypeClient
*/

#include <client/RTypeClient.hpp>
#include <string.h>
#include <arpa/inet.h>
#include <game/utils/ClientUtils.hpp>
#include <thread>
#include <chrono>
#include <iostream>
#include <common/protocol/Protocol.hpp>
#include <common/constants/defines.hpp>
#include <client/network/ClientNetworkManager.hpp>
#include <common/logger/Logger.hpp>
#include <common/error/Error.hpp>

RTypeClient::RTypeClient() : _isRunning(false)
{
}

RTypeClient::~RTypeClient()
{
}

void RTypeClient::init(const char* serverIp, uint16_t port, std::string playerName)
{
    _networkManager = std::make_unique<client::network::ClientNetworkManager>(serverIp, port, this);

    // TODO: initialize game

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
            // TODO: game run
            // if (!_game->runGameLoop(TYPE::CLIENT)) {
            //  stop();
            // break;
            auto incomingPackets = _networkManager->fetchIncoming();
            for (const auto &entry : incomingPackets) {
                // TODO: push back dans la queue du game
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
