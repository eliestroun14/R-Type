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
#include <chrono>
#include <memory>
#include <common/network/NetworkManager.hpp>
#include <common/protocol/Packet.hpp>
#include <game/coordinator/Coordinator.hpp>

class Game {
    public:
        enum class Type : uint8_t {
            SERVER = 0,
            CLIENT = 1,
            STAND_ALONE = 2
        };

        Game(Type type = Type::SERVER);
        ~Game();

        bool runGameLoop(); // process packet + update systems / components + render + packet creation

        void addIncomingPacket(const common::network::ReceivedPacket& packet);
        std::optional<std::pair<common::protocol::Packet, std::optional<uint32_t>>> popOutgoingPacket();

        void setConnected(bool status) { _isConnected = status; }
        bool isConnected() const { return _isConnected; }
        void setRunning(bool status) { _isRunning = status; }
        bool isRunning() const { return _isRunning; }

        // Get coordinator for initialization purposes
        std::shared_ptr<Coordinator> getCoordinator() { return _coordinator; }

        // Server-side: Handle a new player connection
        void onPlayerConnected(uint32_t playerId);

    protected:
        void addOutgoingPacket(const common::protocol::Packet& packet, std::optional<uint32_t> target = std::nullopt);
        std::optional<common::network::ReceivedPacket> popIncomingPacket();

        // Server-side simulation step
        void serverTick(uint64_t elapsedMs);

        // Client-side prediction and reconciliation step
        void clientTick(uint64_t elapsedMs);

    private:
        Type _type;

        // Coordinator manages ECS and packet handling
        std::shared_ptr<Coordinator> _coordinator;

        std::deque<common::network::ReceivedPacket> _incoming;
        std::deque<std::pair<common::protocol::Packet, std::optional<uint32_t>>> _outgoing;

        // Mutexes to protect queue access across threads
        std::mutex _incomingMutex;
        std::mutex _outgoingMutex;

        bool _isConnected = false;
        bool _isRunning = false;

        // Timing for fixed timestep
        std::chrono::steady_clock::time_point _lastTickTime;
        static constexpr uint64_t TICK_RATE_MS = 16; // ~60 FPS
        uint64_t _accumulatedTime = 0;
        
        // Track connected player IDs for spawning existing players to new clients
        std::vector<uint32_t> _connectedPlayers;
};

#endif /* !GAME_HPP_ */
