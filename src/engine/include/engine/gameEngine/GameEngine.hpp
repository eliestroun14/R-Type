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


public:
    /**
     * @brief Initializes the GameEngine and underlying Coordinator.
     */
    void init();


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
};

} // namespace gameEngine

#endif /* !GAMEENGINE_HPP_ */