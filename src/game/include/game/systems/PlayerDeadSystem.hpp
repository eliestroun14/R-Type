/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PlayerDeadSystem
*/

#ifndef PLAYERDEADSYSTEM_HPP_
#define PLAYERDEADSYSTEM_HPP_

#include <engine/GameEngine.hpp>
#include <engine/ecs/system/System.hpp>

class PlayerDeadSystem : public System {
    public:
        PlayerDeadSystem(gameEngine::GameEngine& engine) : _engine(engine) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine& _engine;
};

#endif /* !PLAYERDEADSYSTEM_HPP_ */
