/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PlayerSystem
*/

#ifndef PLAYERSYSTEM_HPP_
#define PLAYERSYSTEM_HPP_

#include <engine/gameEngine/coordinator/Coordinator.hpp>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>

class PlayerSystem : public System{
    public:
        PlayerSystem(Coordinator& coordinator) : _coordinator(coordinator) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        Coordinator& _coordinator;
};

#endif /* !PLAYERSYSTEM_HPP_ */
