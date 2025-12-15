/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** GameEngine
*/

#ifndef GAMEENGINE_HPP_
#define GAMEENGINE_HPP_

#include <memory>

#include <engine/gameEngine/coordinator/Coordinator.hpp>
#include <common/constants/defines.hpp>
#include <common/protocol/Packet.hpp>
#include <common/network/NetworkManager.hpp>

/**
 * @brief High-level game loop orchestrator.
 *
 * Owns a Coordinator and delegates:
 *  - input handling
 *  - system updates
 *  - rendering
 * depending on the network mode (server, client, standalone).
 */
namespace gameEngine {

class GameEngine {
private:
    std::unique_ptr<Coordinator> _coordinator;

    /**
     * @brief Updates all systems via the Coordinator.
     * @param dt Delta time in seconds.
     */
    void update(float dt);

    /**
     * @brief Handles input depending on the network mode.
     * @param type Network mode (SERVER, CLIENT, STANDALONE).
     */
    void processInput(NetworkType type);

    /**
     * @brief Triggers rendering depending on the network mode.
     * @param type Network mode (SERVER, CLIENT, STANDALONE).
     */
    void render(NetworkType type);

    /**
     * @brief Register Components for the Game.
     * @param type Network mode (SERVER, CLIENT, STANDALONE).
     */
    void registerComponents();

    /**
     * @brief Register Systems for the Game.
     * @param type Network mode (SERVER, CLIENT, STANDALONE).
     */
    void registerSystems();

    /**
     * @brief Create player Entity for the Game.
     * @param type Network mode (SERVER, CLIENT, STANDALONE).
     */
    void createPlayer();

    /**
     * @brief Create background Entity(ies) for the Game.
     * @param type Network mode (SERVER, CLIENT, STANDALONE).
     */
    void createBackground();

    /**
     * @brief Create enimies Entities for the Game.
     * @param type Network mode (SERVER, CLIENT, STANDALONE).
     */
    void createEnemies();

    /**
     * @brief Create level for the Game.
     * @param type Network mode (SERVER, CLIENT, STANDALONE).
     */
    void createLevel();

    /**
     * @brief Broadcast Transform component snapshots to all clients (Server-only).
     * @param networkManager Reference to the network manager
     */
    void broadcastTransformSnapshots(common::network::INetworkManager& networkManager);

    /**
     * @brief Broadcast Velocity component snapshots to all clients (Server-only).
     * @param networkManager Reference to the network manager
     */
    void broadcastVelocitySnapshots(common::network::INetworkManager& networkManager);

    /**
     * @brief Broadcast Health component snapshots to all clients (Server-only).
     * @param networkManager Reference to the network manager
     */
    void broadcastHealthSnapshots(common::network::INetworkManager& networkManager);

    /**
     * @brief Broadcast Animation component snapshots to all clients (Server-only).
     * @param networkManager Reference to the network manager
     */
    void broadcastAnimationSnapshots(common::network::INetworkManager& networkManager);

    /**
     * @brief Broadcast Weapon component snapshots to all clients (Server-only).
     * @param networkManager Reference to the network manager
     */
    void broadcastWeaponSnapshots(common::network::INetworkManager& networkManager);


public:
    /**
     * @brief Initializes the GameEngine and underlying Coordinator.
     */
    void init();

    /**
     * @brief Get access to the Coordinator
     */
    Coordinator* getCoordinator() { return _coordinator.get(); }

    void initRender();

    /**
     * @brief Executes a full frame: input → update → render.
     * @param dt Delta time.
     * @param type Network mode.
     */
    void process(float dt, NetworkType type, std::vector<common::protocol::Packet> &packetsToProcess, uint64_t elapsedMs);

    void processInput();

    void handlePacket(NetworkType type, const std::vector<common::protocol::Packet> &packetsToProcess, uint64_t elapsedMs);

    void buildPacketBasedOnStatus(NetworkType type, uint64_t elapsedMs, std::vector<common::protocol::Packet> &outgoingPackets);
    /**
     * @brief Broadcasts entity state snapshots to all connected clients (Server-only).
     * @param networkManager Reference to the server's NetworkManager
     * @param type Network mode (only broadcasts if type == SERVER)
     */
    void broadcastSnapshots(common::network::INetworkManager& networkManager, NetworkType type);
};

} // namespace gameEngine

#endif /* !GAMEENGINE_HPP_ */