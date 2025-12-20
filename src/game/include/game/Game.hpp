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
#include <mutex>
#include <cstdint>
#include <common/network/NetworkManager.hpp>
#include <common/protocol/Packet.hpp>

class Game {
    public:
        enum class Type : uint8_t {
            SERVER = 0,
            CLIENT = 1,
            STAND_ALONE = 2
        };

        Game(Type type = Type::SERVER);
        ~Game();

        bool runGameLoop();

        void addIncomingPacket(const common::network::ReceivedPacket& packet);
        std::optional<std::pair<common::protocol::Packet, std::optional<uint32_t>>> popOutgoingPacket();

        void setConnected(bool status) { _isConnected = status; }
        bool isConnected() const { return _isConnected; }
        void setRunning(bool status) { _isRunning = status; }
        bool isRunning() const { return _isRunning; }

    protected:
        void addOutgoingPacket(const common::protocol::Packet& packet, std::optional<uint32_t> target = std::nullopt);
        std::optional<common::network::ReceivedPacket> popIncomingPacket();

    private:
        Type _type;

        std::deque<common::network::ReceivedPacket> _incoming;
        std::deque<std::pair<common::protocol::Packet, std::optional<uint32_t>>> _outgoing;

        // Mutexes to protect queue access across threads
        std::mutex _incomingMutex;
        std::mutex _outgoingMutex;

        bool _isConnected = false;
        bool _isRunning = false;
        bool _packetsSent = false; //TODO: remove here for debug
};

#endif /* !GAME_HPP_ */
