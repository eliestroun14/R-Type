/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** BackgroundSystem
*/

#ifndef BACKGROUNDSYSTEM_HPP_
#define BACKGROUNDSYSTEM_HPP_

#include <engine/gameEngine/coordinator/Coordinator.hpp>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>

class BackgroundSystem : public System {
    public:
        BackgroundSystem(Coordinator& coordinator) : _coordinator(coordinator) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        Coordinator& _coordinator;
};

#endif /* !BACKGROUNDSYSTEM_HPP_ */
