/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game
*/

#include "Game.hpp"
#include <iostream>

Game::Game()
{
}

Game::~Game()
{
}

bool Game::runGameLoop()
{
    std::cout << "Running game loop..." << std::endl;
    return true;
}

void Game::addIncomingPacket(const common::network::ReceivedPacket &packet)
{
    _incoming.push_back(packet);
}

std::optional<std::pair<common::protocol::Packet, std::optional<uint32_t>>> Game::popOutgoingPacket()
{
    if (!_outgoing.empty()) {
        auto packet = _outgoing.front();
        _outgoing.pop_front();
        return packet;
    }
    return std::nullopt;
}

void Game::addOutgoingPacket(const common::protocol::Packet &packet, std::optional<uint32_t> target)
{
    _outgoing.emplace_back(packet, target);
}

std::optional<common::network::ReceivedPacket> Game::popIncomingPacket()
{
    if (!_incoming.empty()) {
        auto packet = _incoming.front();
        _incoming.pop_front();
        return packet;
    }
    return std::nullopt;
}
