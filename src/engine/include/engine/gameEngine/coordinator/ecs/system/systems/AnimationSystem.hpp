/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** AnimationSystem
*/

#ifndef ANIMATIONSYSTEM_HPP_
#define ANIMATIONSYSTEM_HPP_

#include <engine/gameEngine/GameEngine.hpp>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>

class AnimationSystem : public System {
    public:
        AnimationSystem(gameEngine::GameEngine engine) : _engine(engine) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine _engine;
};

#endif /* !ANIMATIONSYSTEM_HPP_ */
