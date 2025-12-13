/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** AnimationSystem
*/

#ifndef ANIMATIONSYSTEM_HPP_
#define ANIMATIONSYSTEM_HPP_

#include <engine/gameEngine/coordinator/Coordinator.hpp>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>

class AnimationSystem : public System {
    public:
        AnimationSystem(Coordinator& coordinator) : _coordinator(coordinator) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        Coordinator& _coordinator;
};

#endif /* !ANIMATIONSYSTEM_HPP_ */
