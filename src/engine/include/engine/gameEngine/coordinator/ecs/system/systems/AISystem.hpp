/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** AISystem
*/

#pragma once

#include <engine/gameEngine/coordinator/ecs/system/System.hpp>
#include <engine/gameEngine/GameEngine.hpp>

class AISystem : public System {
public:
    AISystem(gameEngine::GameEngine engine) : _engine(engine) {}
    void onUpdate(float dt) override;

private:
    gameEngine::GameEngine& _engine;
};
