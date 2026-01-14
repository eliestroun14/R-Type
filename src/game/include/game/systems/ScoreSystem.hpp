/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** ScoreSystem
*/

#ifndef SHOOTSYSTEM_HPP_
#define SHOOTSYSTEM_HPP_

#include <engine/GameEngine.hpp>
#include <engine/ecs/system/System.hpp>
#include <engine/ecs/component/Components.hpp>

/**
 * @class ScoreSystem
 * @brief Manages player scores based on in-game events.
 *
 * Responsibilities:
 *  - Listens for scoring events (e.g., enemy defeated, objective completed)
 *  - Updates player score components accordingly
 *  - Interfaces with other systems to retrieve scoring data
 */
class ScoreSystem : public System {
public:
    /**
     * @brief Constructor.
     * @param engine Reference to the Coordinator.
     */
    ScoreSystem(gameEngine::GameEngine& engine) : _engine(engine) {}

    /**
     * @brief Called when the system starts running.
     */
    void onStartRunning() override;

    /**
     * @brief Called every update cycle.
     * @param dt Delta time in seconds.
     */
    void onUpdate(float dt) override;

private:
    gameEngine::GameEngine& _engine;
};