/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game
*/

#ifndef GAME_HPP_
#define GAME_HPP_
#include <deque>
#include <optional>
#include <common/network/NetworkManager.hpp>
#include <common/protocol/Packet.hpp>

class Game {
    public:
        Game();
        ~Game();

        bool runGameLoop();

        void addIncomingPacket(const common::network::ReceivedPacket& packet);
        std::optional<std::pair<common::protocol::Packet, std::optional<uint32_t>>> popOutgoingPacket();

    protected:
        void addOutgoingPacket(const common::protocol::Packet& packet, std::optional<uint32_t> target = std::nullopt);
        std::optional<common::network::ReceivedPacket> popIncomingPacket();

    private:
        std::deque<common::network::ReceivedPacket> _incoming;
        std::deque<std::pair<common::protocol::Packet, std::optional<uint32_t>>> _outgoing;
};

#endif /* !GAME_HPP_ */
