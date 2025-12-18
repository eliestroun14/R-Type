/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** BackgroundSystem
*/

#ifndef BACKGROUNDSYSTEM_HPP_
#define BACKGROUNDSYSTEM_HPP_

#include <engine/gameEngine/GameEngine.hpp>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>

class BackgroundSystem : public System {
    public:
        BackgroundSystem(gameEngine::GameEngine& engine) : _engine(engine) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine& _engine;
};

#endif /* !BACKGROUNDSYSTEM_HPP_ */
