/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** LevelTimerSystem
*/

#ifndef LEVELTIMERYSTEM_HPP_
#define LEVELTIMERYSTEM_HPP_

#include <engine/GameEngine.hpp>
#include <engine/ecs/system/System.hpp>
#include <engine/ecs/component/Components.hpp>

class LevelTimerSystem : public System {
    public:
        LevelTimerSystem(gameEngine::GameEngine& engine) : _engine(engine) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine& _engine;
};

#endif /* !LEVELTIMERYSTEM_HPP_ */
