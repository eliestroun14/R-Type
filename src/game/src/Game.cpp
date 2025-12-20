/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game
*/

#include <game/Game.hpp>
#include <iostream>
#include <common/protocol/Protocol.hpp>
#include <common/constants/defines.hpp>
#include <random>
#include <common/protocol/PacketManager.hpp>
#include <common/logger/Logger.hpp>

Game::Game(Type type)
    : _type(type), _isRunning(true), _isConnected(false)
{
}

Game::~Game()
{
    _isRunning = false;
}

bool Game::runGameLoop()
{
    std::cout << "Running game loop..." << std::endl;
    return true;
}

void Game::addIncomingPacket(const common::network::ReceivedPacket &packet)
{
    std::lock_guard<std::mutex> guard(_incomingMutex);
    _incoming.push_back(packet);
    LOG_DEBUG("Game: incoming queue size={}", _incoming.size());
}

std::optional<std::pair<common::protocol::Packet, std::optional<uint32_t>>> Game::popOutgoingPacket()
{
    std::lock_guard<std::mutex> guard(_outgoingMutex);
    if (!_outgoing.empty()) {
        auto packet = _outgoing.front();
        _outgoing.pop_front();
        LOG_DEBUG("Game: popped outgoing packet type={} remaining={}", static_cast<int>(packet.first.header.packet_type), _outgoing.size());
        return packet;
    }
    return std::nullopt;
}

void Game::addOutgoingPacket(const common::protocol::Packet &packet, std::optional<uint32_t> target)
{
    std::lock_guard<std::mutex> guard(_outgoingMutex);
    _outgoing.emplace_back(packet, target);
    LOG_DEBUG("Game: outgoing queue size={}", _outgoing.size());
}

std::optional<common::network::ReceivedPacket> Game::popIncomingPacket()
{
    std::lock_guard<std::mutex> guard(_incomingMutex);
    if (!_incoming.empty()) {
        auto packet = _incoming.front();
        _incoming.pop_front();
        LOG_DEBUG("Game: popped incoming packet type={} remaining={}", static_cast<int>(packet.packet.header.packet_type), _incoming.size());
        return packet;
    }
    return std::nullopt;
}
