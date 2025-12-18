/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PlayerSystem
*/

#ifndef PLAYERSYSTEM_HPP_
#define PLAYERSYSTEM_HPP_

#include <engine/gameEngine/GameEngine.hpp>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>

class PlayerSystem : public System{
    public:
        PlayerSystem(gameEngine::GameEngine& engine) : _engine(engine), _lastDirection(2) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine& _engine;
        int _lastDirection;  // Track previous direction to detect changes
};

#endif /* !PLAYERSYSTEM_HPP_ */
