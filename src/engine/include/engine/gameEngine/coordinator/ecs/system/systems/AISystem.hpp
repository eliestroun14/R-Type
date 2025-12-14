/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** AISystem
*/

#pragma once

#include <engine/gameEngine/coordinator/ecs/system/System.hpp>
#include <engine/gameEngine/coordinator/Coordinator.hpp>

class AISystem : public System {
public:
    AISystem(Coordinator& coord) : _coord(coord) {}
    void onUpdate(float dt) override;

private:
    Coordinator& _coord;
};
